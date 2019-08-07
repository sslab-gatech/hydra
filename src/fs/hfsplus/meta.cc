#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/sysctl.h>
#include <sys/types.h>
#include <stdint.h>

#include <set>

#include "meta.hh"


DeviceInfo_t *OpenDevice(const char *devname) {
    DeviceInfo_t *retval = NULL;
    int fd;
    DeviceInfo_t dev = { 0 };
    struct stat st;

    if (stat(devname, &st) == -1) {
        fprintf(stderr, "cannot open device %s", devname);
    }
    
    dev.size = st.st_size;
    dev.blockSize = 4096;
    dev.blockCount = dev.size / dev.blockSize;
    dev.devname = strdup(devname);

    fd = open(dev.devname, O_RDONLY);
    dev.fd = fd;

    retval = (DeviceInfo_t *)malloc(sizeof(*retval));
    *retval = dev;
    return retval;
}

VolumeDescriptor_t *
VolumeInfo(DeviceInfo_t *devp)
{
	uint8_t buffer[devp->blockSize];
	VolumeDescriptor_t *vdp = NULL, vd = { 0 };
	ssize_t rv;

	vd.priOffset = 1024;	// primary volume header is at 1024 bytes
	vd.altOffset = devp->size - 1024;	// alternate header is 1024 bytes from the end

	rv = GetBlock(devp, vd.priOffset, buffer);
	if (rv == -1) {
		fprintf(stderr, "cannot get primary volume header for device %s", devp->devname);
	}
	vd.priHeader = *(HFSPlusVolumeHeader*)buffer;

	rv = GetBlock(devp, vd.altOffset, buffer);
	if (rv == -1) {
		fprintf(stderr, "cannot get alternate volume header for device %s", devp->devname);
	}
	vd.altHeader = *(HFSPlusVolumeHeader*)buffer;

	vdp = (VolumeDescriptor_t *)malloc(sizeof(*vdp));
	*vdp = vd;

	return vdp;
}

/*
 * Only two (currently) types of signatures are valid: H+ and HX.
 */
static int
IsValidSigWord(uint16_t word) {
	if (word == kHFSPlusSigWord ||
	    word == kHFSXSigWord)
		return 1;
	return 0;
}

int
AddHeaders(VolumeObjects_t *vop, int roundBlock)
{
	int retval = 1;
	HFSPlusVolumeHeader *hp;
	uint8_t buffer[vop->devp->blockSize];
	ssize_t rv;

	hp = &vop->vdp->priHeader;

	if (IsValidSigWord(S16(hp->signature)) == 0) {
		fprintf(stderr, "primary volume header signature = %x, invalid", S16(hp->signature));
		retval = 0;
	}
	if (roundBlock) {
		AddExtent(vop, 1024 / vop->devp->blockSize, vop->devp->blockSize);
	} else {
		AddExtent(vop, 1024, 512);
	}

	hp = &vop->vdp->altHeader;

	if (IsValidSigWord(S16(hp->signature)) == 0) {
		fprintf(stderr, "alternate volume header signature = %x, invalid", S16(hp->signature));
		retval = 0;
	}
	if (roundBlock) {
		AddExtent(vop, (vop->vdp->altOffset / vop->devp->blockSize) * vop->devp->blockSize, vop->devp->blockSize);
	} else {
		AddExtent(vop, vop->vdp->altOffset, 512);
	}

done:
	return retval;
}

void
AddFileExtents(VolumeObjects_t *vop)
{
	int useAlt = 0;
#define ADDEXTS(vop, file, fid)			\
	do { \
		off_t pSize = S32(vop->vdp->priHeader.blockSize);	\
		off_t aSize = S32(vop->vdp->altHeader.blockSize);	\
		int i;							\
		printf("Adding " #file " extents\n");		\
		for (i = 0; i < kHFSPlusExtentDensity; i++) {		\
			HFSPlusExtentDescriptor *ep = &vop->vdp->priHeader. file .extents[i]; \
			HFSPlusExtentDescriptor *ap = &vop->vdp->altHeader. file .extents[i]; \
			printf("\tExtent <%u, %u>\n", S32(ep->startBlock), S32(ep->blockCount)); \
			if (ep->startBlock && ep->blockCount) {		\
				AddExtentForFile(vop, S32(ep->startBlock) * pSize, S32(ep->blockCount) * pSize, fid); \
				if (memcmp(ep, ap, sizeof(*ep)) != 0) { \
					AddExtentForFile(vop, S32(ap->startBlock) * aSize, S32(ap->blockCount) * aSize, fid); \
					useAlt = 1;			\
				}					\
			}						\
		}							\
	} while (0)

	ADDEXTS(vop, allocationFile, kHFSAllocationFileID);
	ADDEXTS(vop, extentsFile, kHFSExtentsFileID);
	ADDEXTS(vop, catalogFile, kHFSCatalogFileID);
	ADDEXTS(vop, attributesFile, kHFSAttributesFileID);
	ADDEXTS(vop, startupFile, kHFSStartupFileID);

#undef ADDEXTS

	ScanExtents(vop, 0);
	if (useAlt)
		ScanExtents(vop, useAlt);

	return;
}

/*
 * Perform a (potentially) unaligned read from a given input device.
 */
ssize_t
UnalignedRead(DeviceInfo_t *devp, void *buffer, size_t size, off_t offset)
{
	ssize_t nread = -1;
	size_t readSize = ((size + devp->blockSize - 1) / devp->blockSize) * devp->blockSize;
	off_t baseOffset = (offset / devp->blockSize) * devp->blockSize;
	size_t off = offset - baseOffset;
	char *tmpbuf = NULL;
	
	if ((baseOffset == offset) && (readSize == size)) {
		/*
		 * The read is already properly aligned, so call pread.
		 */
		return pread(devp->fd, buffer, size, offset);
	}
	
	tmpbuf = (char *)malloc(readSize);
	if (!tmpbuf) {
		goto done;
	}
	
	nread = pread(devp->fd, tmpbuf, readSize, baseOffset);
	if (nread == -1) {
		goto done;
	}
	
	nread -= off;
	if (nread > (ssize_t)size) {
		nread = size;
	}
	memcpy(buffer, tmpbuf + off, nread);
	
done:
	free(tmpbuf);
	return nread;
}

static int
ScanCatalogNode(VolumeObjects_t *vop, uint8_t *buffer, size_t nodeSize, extent_handler_t handler)
{
	BTNodeDescriptor *ndp = (BTNodeDescriptor *)buffer;
	uint16_t *indices = (uint16_t*)(buffer + nodeSize);
	size_t counter;
	off_t blockSize = S32(vop->vdp->priHeader.blockSize);
	int retval = 0;

	if (ndp->kind != kBTLeafNode)	// Skip if it's not a leaf node
		return 0;

	printf("%s:  scanning catalog node\n", __FUNCTION__);

	for (counter = 1; counter <= S16(ndp->numRecords); counter++) {
		// Need to get past the end of the key
		uint16_t recOffset = S16(indices[-counter]);
		HFSPlusCatalogKey *keyp = (HFSPlusCatalogKey*)(buffer + recOffset);
		size_t keyLength = S16(keyp->keyLength);
		// Add two because the keyLength field is not included.
		HFSPlusCatalogFile *fp = (HFSPlusCatalogFile*)(((uint8_t*)keyp) +  2 + keyLength + (keyLength & 1));

		if (S16(fp->recordType) != kHFSPlusFileRecord) {
				printf("%s:  skipping node record %zu because it is type %#x, at offset %u keyLength %zu\n", __FUNCTION__, counter, S16(fp->recordType), recOffset, keyLength);
			continue;
		 }

		printf("%s:  node record %zu, file id = %u\n", __FUNCTION__, counter, S32(fp->fileID));
		// if (S32(fp->userInfo.fdType) == kSymLinkFileType &&
		//    S32(fp->userInfo.fdCreator) == kSymLinkCreator)
		{
			unsigned int fid = S32(fp->fileID);
			HFSPlusExtentDescriptor *extPtr = fp->dataFork.extents;
			int i;

			for (i = 0; i < 8; i++) {
				if (extPtr[i].startBlock &&
				    extPtr[i].blockCount) {
					off_t start = blockSize * S32(extPtr[i].startBlock);
					off_t length = blockSize * S32(extPtr[i].blockCount);
					retval = handler(vop, fid, start, length);
					if (retval != 0)
						return retval;
				} else {
					break;
				}
			}
		}
	}
	return retval;
}

static int
ScanAttrNode(VolumeObjects_t *vop, uint8_t *buffer, size_t nodeSize, extent_handler_t handler)
{
	BTNodeDescriptor *ndp = (BTNodeDescriptor *)buffer;
	uint16_t *indices = (uint16_t*)(buffer + nodeSize);
	size_t counter;
	off_t blockSize = S32(vop->vdp->priHeader.blockSize);
	int retval = 0;

	if (ndp->kind != kBTLeafNode)
		return 0;	// Skip if it's not a leaf node

	/*
	 * Look for records of type kHFSPlusForkData and kHFSPlusAttrExtents
	 */
	for (counter = 1; counter <= S16(ndp->numRecords); counter++) {
		// Need to get past the end of the key
		unsigned int fid;
		HFSPlusAttrKey *keyp = (HFSPlusAttrKey*)(buffer + S16(indices[-counter]));
		size_t keyLength = S16(keyp->keyLength);
		// Add two because the keyLength field is not included.
		HFSPlusAttrRecord *ap = (HFSPlusAttrRecord*)(((uint8_t*)keyp) + 2 + keyLength + (keyLength & 1));
		HFSPlusExtentDescriptor *theExtents = NULL;
		switch (S32(ap->recordType)) {
		case kHFSPlusAttrForkData:
			theExtents = ap->forkData.theFork.extents;
			break;
		case kHFSPlusAttrExtents:
			theExtents = ap->overflowExtents.extents;
			break;
		default:
			break;
		}
		if (theExtents != NULL) {
			HFSPlusExtentDescriptor *extPtr = theExtents;
			int i;
			fid = S32(keyp->fileID);

			for (i = 0; i < 8; i++) {
				if (extPtr[i].startBlock &&
				    extPtr[i].blockCount) {
					off_t start = blockSize * S32(extPtr[i].startBlock);
					off_t length = blockSize * S32(extPtr[i].blockCount);
					retval = handler(vop, fid, start, length);
					if (retval != 0)
						return retval;
				} else {
					break;
				}
			}
		}
	}
	return retval;
}

int ExtentHandler(VolumeObjects_t *vop, int fid, off_t start, off_t len) 
{
	AddExtentForFile(vop, start, len, fid);
	// fprintf(stderr, "AddExtentForFile(%p, %llu, %llu, %u)\n", vop, start, len, fid);
	return 0;
}

int
FindOtherMetadata(VolumeObjects_t *vop, extent_handler_t handler)
{
	size_t catNodeSize = 0, attrNodeSize = 0;
	off_t node0_location = 0;
	uint8_t *tBuffer;
	BTHeaderRec *hdp;
	BTNodeDescriptor *ndp;
	int retval = 0;

	tBuffer = (uint8_t *)calloc(1, vop->devp->blockSize);
	if (tBuffer == NULL) {
		fprintf(stderr, "Could not allocate memory to collect extra metadata");
		goto done;
	}
	/*
	 * First, do the catalog file
	 */
	if (vop->vdp->priHeader.catalogFile.logicalSize) {

		node0_location = S32(vop->vdp->priHeader.catalogFile.extents[0].startBlock);
		node0_location = node0_location * S32(vop->vdp->priHeader.blockSize);
		if (GetBlock(vop->devp, node0_location, tBuffer) == -1) {
			fprintf(stderr, "Could not read catalog header node");
		} else {
			ndp = (BTNodeDescriptor*)tBuffer;
			hdp = (BTHeaderRec*)(tBuffer + sizeof(BTNodeDescriptor));
			
			if (ndp->kind != kBTHeaderNode) {
				fprintf(stderr, "Did not read header node for catalog as expected");
			} else {
				catNodeSize = S16(hdp->nodeSize);
			}
		}
	}
	/*
	 * Now, the attributes file.
	 */
	if (vop->vdp->priHeader.attributesFile.logicalSize) {

		node0_location = S32(vop->vdp->priHeader.attributesFile.extents[0].startBlock);
		node0_location = node0_location * S32(vop->vdp->priHeader.blockSize);
		if (GetBlock(vop->devp, node0_location, tBuffer) == -1) {
			fprintf(stderr, "Could not read attributes file header node");
		} else {
			ndp = (BTNodeDescriptor*)tBuffer;
			hdp = (BTHeaderRec*)(tBuffer + sizeof(BTNodeDescriptor));
			
			if (ndp->kind != kBTHeaderNode) {
				fprintf(stderr, "Did not read header node for attributes file as expected");
			} else {
				attrNodeSize = S16(hdp->nodeSize);
			}
		}
	}

	printf("Catalog node size = %zu, attributes node size = %zu\n", catNodeSize, attrNodeSize);

	/*
	 * We start reading the extents now.
	 *
	 * This is a lot of duplicated code, unfortunately.
	 */
	ExtentList_t *exts;
	for (exts = vop->list;
	     exts;
	     exts = exts->next) {
		size_t indx;
		
		for (indx = 0; indx < exts->count; indx++) {
			off_t start = exts->extents[indx].base;
			off_t len = exts->extents[indx].length;
			off_t nread = 0;
			if (exts->extents[indx].fid == 0) {
				continue;	// Unknown file, skip
			} else {
				printf("%s:  fid = %u, start = %lu, len = %lu\n", __FUNCTION__, exts->extents[indx].fid, start, len);
				while (nread < len) {
					size_t bufSize;
					uint8_t *buffer;
					bufSize = MIN(len - nread, 1024 * 1024);	// Read 1mbyte max
					buffer = (uint8_t *)calloc(1, bufSize);
					if (buffer == NULL) {
						fprintf(stderr, "Cannot allocate %zu bytes for buffer, skipping node scan", bufSize);
					} else {
						ssize_t t = UnalignedRead(vop->devp, buffer, bufSize, start + nread);
						if (t != bufSize) {
							fprintf(stderr, "Attempted to read %zu bytes, only read %zd, skipping node scan", bufSize, t);
						} else {
							uint8_t *curPtr = buffer, *endPtr = (buffer + bufSize);
							size_t nodeSize = 0;
							int (*func)(VolumeObjects_t *, uint8_t *, size_t, extent_handler_t) = NULL;
							if (exts->extents[indx].fid == kHFSCatalogFileID) {
								func = ScanCatalogNode;
								nodeSize = catNodeSize;
							} else if (exts->extents[indx].fid == kHFSAttributesFileID) {
								func = ScanAttrNode;
								nodeSize = attrNodeSize;
							}
							if (func) {
								while (curPtr < endPtr && retval == 0) {
									retval = (*func)(vop, curPtr, nodeSize, handler);
									curPtr += nodeSize;
								}
							}
						}
						free(buffer);
					}
					if (retval != 0)
						goto done;
					nread += bufSize;
				}
			}
		}
	}

done:
	if (tBuffer)
		free(tBuffer);
	return retval;
}


void find_meta(std::set<uint64_t> &meta_blocks, const char *src) {

	DeviceInfo_t *devp = NULL;
	VolumeDescriptor_t *vdp = NULL;
	VolumeObjects_t *vop = NULL;

    meta_blocks.clear();

    // Start by opening the input device
    devp = OpenDevice(src);
    if (devp == NULL) {
        fprintf(stderr, "cannot get device information for %s", src);
        _exit(1);
    }
                      
    // Get the volume information.
    vdp = VolumeInfo(devp);

	// Start creating the in-core volume list
	vop = InitVolumeObject(devp, vdp);

	// Add the volume headers
	if (AddHeaders(vop, 0) == 0) {
		fprintf(stderr, "Invalid volume header(s) for %s", src);
		_exit(1);
	}

	AddFileExtents(vop);

	FindOtherMetadata(vop, ExtentHandler);	
	
	for (ExtentList_t *exts = vop->list;
	     exts;
	     exts = exts->next) {
		int indx;
		for (indx = 0; indx < exts->count; indx++) {
			printf("\t\t<%ld, %ld> (file %u)\n", exts->extents[indx].base, exts->extents[indx].length, exts->extents[indx].fid);
            uint64_t base = exts->extents[indx].base;
            uint64_t length = exts->extents[indx].length;

            for (uint64_t i = 0; i < length / 512; i++) {
                // printf("%ld\n", base / 512 + i);
                meta_blocks.insert(base / 512 + i);
            }
		}
	}

	ReleaseVolumeObjects(vop);
}

/*
int main(int argc, char *argv[]) {
    
    char *src = argv[1];
    std::set<uint64_t> meta_blocks;

    find_meta(meta_blocks, src);

	return 0;
}
*/


