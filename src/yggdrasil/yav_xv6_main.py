import pstats, cProfile
import yav_dirimpl_fuse

def run():
    yav_dirimpl_fuse.main()

def profile():
    cProfile.runctx("yav_dirimpl_fuse.main()", globals(), locals(), "Profile.prof")
    s = pstats.Stats("Profile.prof")
    s.strip_dirs().sort_stats("time").print_stats()


if __name__ == '__main__':
    run()
    # profile()

