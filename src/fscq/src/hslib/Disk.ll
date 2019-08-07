target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64"
target triple = "x86_64-linux-gnu"
declare ccc i8* @memcpy$def(i8*, i8*, i64)
declare ccc i8* @memmove$def(i8*, i8*, i64)
declare ccc i8* @memset$def(i8*, i64, i64)
declare ccc i64 @newSpark$def(i8*, i8*)
!0 = !{!"top", null}
!1 = !{!"stack", !0}
!2 = !{!"heap", !0}
!3 = !{!"rx", !2}
!4 = !{!"base", !0}
!5 = !{!"other", !0}
%__stginit_Disk_struct = type <{}>
@__stginit_Disk$def = internal global %__stginit_Disk_struct<{}>
@__stginit_Disk = alias i8* bitcast (%__stginit_Disk_struct* @__stginit_Disk$def to i8*)
%Disk_zdWFL_closure_struct = type <{i64}>
@Disk_zdWFL_closure$def = internal global %Disk_zdWFL_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdWFL_info$def to i64)}>
@Disk_zdWFL_closure = alias i8* bitcast (%Disk_zdWFL_closure_struct* @Disk_zdWFL_closure$def to i8*)
@Disk_zdWFL_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdWFL_info$def to i8*)
define ghccc void @Disk_zdWFL_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934607, i64 0, i64 15}>
{
cbbg:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnbbx = load i64*, i64** %Sp_Var
  %lnbby = getelementptr inbounds i64, i64* %lnbbx, i32 -2
  %lnbbz = ptrtoint i64* %lnbby to i64
  %lnbbA = icmp ult i64 %lnbbz, %SpLim_Arg
  br i1 %lnbbA, label %cbbp, label %cbbq
cbbq:
  %lnbbC = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbbd_info$def to i64
  %lnbbB = load i64*, i64** %Sp_Var
  %lnbbD = getelementptr inbounds i64, i64* %lnbbB, i32 -2
  store i64 %lnbbC, i64* %lnbbD, !tbaa !1
  %lnbbE = load i64, i64* %R2_Var
  store i64 %lnbbE, i64* %R1_Var
  %lnbbG = load i64, i64* %R3_Var
  %lnbbF = load i64*, i64** %Sp_Var
  %lnbbH = getelementptr inbounds i64, i64* %lnbbF, i32 -1
  store i64 %lnbbG, i64* %lnbbH, !tbaa !1
  %lnbbI = load i64*, i64** %Sp_Var
  %lnbbJ = getelementptr inbounds i64, i64* %lnbbI, i32 -2
  %lnbbK = ptrtoint i64* %lnbbJ to i64
  %lnbbL = inttoptr i64 %lnbbK to i64*
  store i64* %lnbbL, i64** %Sp_Var
  %lnbbM = load i64, i64* %R1_Var
  %lnbbN = and i64 %lnbbM, 7
  %lnbbO = icmp ne i64 %lnbbN, 0
  br i1 %lnbbO, label %ubbw, label %cbbe
cbbe:
  %lnbbQ = load i64, i64* %R1_Var
  %lnbbR = inttoptr i64 %lnbbQ to i64*
  %lnbbS = load i64, i64* %lnbbR, !tbaa !3
  %lnbbT = inttoptr i64 %lnbbS to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbbU = load i64*, i64** %Sp_Var
  %lnbbV = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbbT( i64* %Base_Arg, i64* %lnbbU, i64* %Hp_Arg, i64 %lnbbV, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubbw:
  %lnbbW = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbbd_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbbX = load i64*, i64** %Sp_Var
  %lnbbY = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbbW( i64* %Base_Arg, i64* %lnbbX, i64* %Hp_Arg, i64 %lnbbY, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbbp:
  %lnbbZ = load i64, i64* %R3_Var
  store i64 %lnbbZ, i64* %R3_Var
  %lnbc0 = load i64, i64* %R2_Var
  store i64 %lnbc0, i64* %R2_Var
  %lnbc1 = ptrtoint %Disk_zdWFL_closure_struct* @Disk_zdWFL_closure$def to i64
  store i64 %lnbc1, i64* %R1_Var
  %lnbc2 = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lnbc3 = bitcast i64* %lnbc2 to i64*
  %lnbc4 = load i64, i64* %lnbc3, !tbaa !4
  %lnbc5 = inttoptr i64 %lnbc4 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbc6 = load i64*, i64** %Sp_Var
  %lnbc7 = load i64, i64* %R1_Var
  %lnbc8 = load i64, i64* %R2_Var
  %lnbc9 = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbc5( i64* %Base_Arg, i64* %lnbc6, i64* %Hp_Arg, i64 %lnbc7, i64 %lnbc8, i64 %lnbc9, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbbd_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbbd_info$def to i8*)
define internal ghccc void @cbbd_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 1, i64 32}>
{
cbbd:
  %lsb0Y = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbca = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbbj_info$def to i64
  %lnbcb = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lnbca, i64* %lnbcb, !tbaa !1
  %lnbcc = load i64, i64* %R1_Var
  store i64 %lnbcc, i64* %lsb0Y
  %lnbcd = getelementptr inbounds i64, i64* %Sp_Arg, i32 1
  %lnbce = bitcast i64* %lnbcd to i64*
  %lnbcf = load i64, i64* %lnbce, !tbaa !1
  store i64 %lnbcf, i64* %R1_Var
  %lnbcg = load i64, i64* %lsb0Y
  %lnbch = getelementptr inbounds i64, i64* %Sp_Arg, i32 1
  store i64 %lnbcg, i64* %lnbch, !tbaa !1
  %lnbci = load i64, i64* %R1_Var
  %lnbcj = and i64 %lnbci, 7
  %lnbck = icmp ne i64 %lnbcj, 0
  br i1 %lnbck, label %ubbv, label %cbbk
cbbk:
  %lnbcm = load i64, i64* %R1_Var
  %lnbcn = inttoptr i64 %lnbcm to i64*
  %lnbco = load i64, i64* %lnbcn, !tbaa !3
  %lnbcp = inttoptr i64 %lnbco to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbcq = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbcp( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbcq, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubbv:
  %lnbcr = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbbj_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbcs = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbcr( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbcs, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbbj_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbbj_info$def to i8*)
define internal ghccc void @cbbj_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 1, i64 32}>
{
cbbj:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnbct = load i64*, i64** %Hp_Var
  %lnbcu = getelementptr inbounds i64, i64* %lnbct, i32 3
  %lnbcv = ptrtoint i64* %lnbcu to i64
  %lnbcw = inttoptr i64 %lnbcv to i64*
  store i64* %lnbcw, i64** %Hp_Var
  %lnbcx = load i64*, i64** %Hp_Var
  %lnbcy = ptrtoint i64* %lnbcx to i64
  %lnbcz = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnbcA = bitcast i64* %lnbcz to i64*
  %lnbcB = load i64, i64* %lnbcA, !tbaa !4
  %lnbcC = icmp ugt i64 %lnbcy, %lnbcB
  br i1 %lnbcC, label %cbbu, label %cbbt
cbbt:
  %lnbcE = ptrtoint i8* @Disk_FL_con_info to i64
  %lnbcD = load i64*, i64** %Hp_Var
  %lnbcF = getelementptr inbounds i64, i64* %lnbcD, i32 -2
  store i64 %lnbcE, i64* %lnbcF, !tbaa !2
  %lnbcH = load i64*, i64** %Sp_Var
  %lnbcI = getelementptr inbounds i64, i64* %lnbcH, i32 1
  %lnbcJ = bitcast i64* %lnbcI to i64*
  %lnbcK = load i64, i64* %lnbcJ, !tbaa !1
  %lnbcG = load i64*, i64** %Hp_Var
  %lnbcL = getelementptr inbounds i64, i64* %lnbcG, i32 -1
  store i64 %lnbcK, i64* %lnbcL, !tbaa !2
  %lnbcN = load i64, i64* %R1_Var
  %lnbcM = load i64*, i64** %Hp_Var
  %lnbcO = getelementptr inbounds i64, i64* %lnbcM, i32 0
  store i64 %lnbcN, i64* %lnbcO, !tbaa !2
  %lnbcQ = load i64*, i64** %Hp_Var
  %lnbcR = ptrtoint i64* %lnbcQ to i64
  %lnbcS = add i64 %lnbcR, -15
  store i64 %lnbcS, i64* %R1_Var
  %lnbcT = load i64*, i64** %Sp_Var
  %lnbcU = getelementptr inbounds i64, i64* %lnbcT, i32 2
  %lnbcV = ptrtoint i64* %lnbcU to i64
  %lnbcW = inttoptr i64 %lnbcV to i64*
  store i64* %lnbcW, i64** %Sp_Var
  %lnbcX = load i64*, i64** %Sp_Var
  %lnbcY = getelementptr inbounds i64, i64* %lnbcX, i32 0
  %lnbcZ = bitcast i64* %lnbcY to i64*
  %lnbd0 = load i64, i64* %lnbcZ, !tbaa !1
  %lnbd1 = inttoptr i64 %lnbd0 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbd2 = load i64*, i64** %Sp_Var
  %lnbd3 = load i64*, i64** %Hp_Var
  %lnbd4 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbd1( i64* %Base_Arg, i64* %lnbd2, i64* %lnbd3, i64 %lnbd4, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbbu:
  %lnbd5 = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lnbd5, !tbaa !4
  %lnbd6 = load i64, i64* %R1_Var
  store i64 %lnbd6, i64* %R1_Var
  %lnbd7 = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbd8 = load i64*, i64** %Sp_Var
  %lnbd9 = load i64*, i64** %Hp_Var
  %lnbda = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbd7( i64* %Base_Arg, i64* %lnbd8, i64* %lnbd9, i64 %lnbda, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_zdWStats_closure_struct = type <{i64}>
@Disk_zdWStats_closure$def = internal global %Disk_zdWStats_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdWStats_info$def to i64)}>
@Disk_zdWStats_closure = alias i8* bitcast (%Disk_zdWStats_closure_struct* @Disk_zdWStats_closure$def to i8*)
@Disk_zdWStats_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdWStats_info$def to i8*)
define ghccc void @Disk_zdWStats_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 12884901911, i64 0, i64 15}>
{
cbds:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R4_Var = alloca i64, i32 1
  store i64 %R4_Arg, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnbdQ = load i64*, i64** %Sp_Var
  %lnbdR = getelementptr inbounds i64, i64* %lnbdQ, i32 -3
  %lnbdS = ptrtoint i64* %lnbdR to i64
  %lnbdT = icmp ult i64 %lnbdS, %SpLim_Arg
  br i1 %lnbdT, label %cbdG, label %cbdH
cbdH:
  %lnbdV = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbdp_info$def to i64
  %lnbdU = load i64*, i64** %Sp_Var
  %lnbdW = getelementptr inbounds i64, i64* %lnbdU, i32 -3
  store i64 %lnbdV, i64* %lnbdW, !tbaa !1
  %lnbdX = load i64, i64* %R2_Var
  store i64 %lnbdX, i64* %R1_Var
  %lnbdZ = load i64, i64* %R3_Var
  %lnbdY = load i64*, i64** %Sp_Var
  %lnbe0 = getelementptr inbounds i64, i64* %lnbdY, i32 -2
  store i64 %lnbdZ, i64* %lnbe0, !tbaa !1
  %lnbe2 = load i64, i64* %R4_Var
  %lnbe1 = load i64*, i64** %Sp_Var
  %lnbe3 = getelementptr inbounds i64, i64* %lnbe1, i32 -1
  store i64 %lnbe2, i64* %lnbe3, !tbaa !1
  %lnbe4 = load i64*, i64** %Sp_Var
  %lnbe5 = getelementptr inbounds i64, i64* %lnbe4, i32 -3
  %lnbe6 = ptrtoint i64* %lnbe5 to i64
  %lnbe7 = inttoptr i64 %lnbe6 to i64*
  store i64* %lnbe7, i64** %Sp_Var
  %lnbe8 = load i64, i64* %R1_Var
  %lnbe9 = and i64 %lnbe8, 7
  %lnbea = icmp ne i64 %lnbe9, 0
  br i1 %lnbea, label %ubdO, label %cbdq
cbdq:
  %lnbec = load i64, i64* %R1_Var
  %lnbed = inttoptr i64 %lnbec to i64*
  %lnbee = load i64, i64* %lnbed, !tbaa !3
  %lnbef = inttoptr i64 %lnbee to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbeg = load i64*, i64** %Sp_Var
  %lnbeh = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbef( i64* %Base_Arg, i64* %lnbeg, i64* %Hp_Arg, i64 %lnbeh, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubdO:
  %lnbei = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbdp_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbej = load i64*, i64** %Sp_Var
  %lnbek = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbei( i64* %Base_Arg, i64* %lnbej, i64* %Hp_Arg, i64 %lnbek, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbdG:
  %lnbel = load i64, i64* %R4_Var
  store i64 %lnbel, i64* %R4_Var
  %lnbem = load i64, i64* %R3_Var
  store i64 %lnbem, i64* %R3_Var
  %lnben = load i64, i64* %R2_Var
  store i64 %lnben, i64* %R2_Var
  %lnbeo = ptrtoint %Disk_zdWStats_closure_struct* @Disk_zdWStats_closure$def to i64
  store i64 %lnbeo, i64* %R1_Var
  %lnbep = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lnbeq = bitcast i64* %lnbep to i64*
  %lnber = load i64, i64* %lnbeq, !tbaa !4
  %lnbes = inttoptr i64 %lnber to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbet = load i64*, i64** %Sp_Var
  %lnbeu = load i64, i64* %R1_Var
  %lnbev = load i64, i64* %R2_Var
  %lnbew = load i64, i64* %R3_Var
  %lnbex = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbes( i64* %Base_Arg, i64* %lnbet, i64* %Hp_Arg, i64 %lnbeu, i64 %lnbev, i64 %lnbew, i64 %lnbex, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbdp_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbdp_info$def to i8*)
define internal ghccc void @cbdp_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 2, i64 32}>
{
cbdp:
  %lsb14 = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbey = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbdv_info$def to i64
  %lnbez = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lnbey, i64* %lnbez, !tbaa !1
  %lnbeC = load i64, i64* %R1_Var
  %lnbeD = add i64 %lnbeC, 7
  %lnbeE = inttoptr i64 %lnbeD to i64*
  %lnbeF = load i64, i64* %lnbeE, !tbaa !3
  store i64 %lnbeF, i64* %lsb14
  %lnbeG = getelementptr inbounds i64, i64* %Sp_Arg, i32 1
  %lnbeH = bitcast i64* %lnbeG to i64*
  %lnbeI = load i64, i64* %lnbeH, !tbaa !1
  store i64 %lnbeI, i64* %R1_Var
  %lnbeJ = load i64, i64* %lsb14
  %lnbeK = getelementptr inbounds i64, i64* %Sp_Arg, i32 1
  store i64 %lnbeJ, i64* %lnbeK, !tbaa !1
  %lnbeL = load i64, i64* %R1_Var
  %lnbeM = and i64 %lnbeL, 7
  %lnbeN = icmp ne i64 %lnbeM, 0
  br i1 %lnbeN, label %ubdN, label %cbdw
cbdw:
  %lnbeP = load i64, i64* %R1_Var
  %lnbeQ = inttoptr i64 %lnbeP to i64*
  %lnbeR = load i64, i64* %lnbeQ, !tbaa !3
  %lnbeS = inttoptr i64 %lnbeR to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbeT = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbeS( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbeT, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubdN:
  %lnbeU = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbdv_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbeV = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbeU( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbeV, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbdv_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbdv_info$def to i8*)
define internal ghccc void @cbdv_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 66, i64 32}>
{
cbdv:
  %lsb16 = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbeW = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbdA_info$def to i64
  %lnbeX = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lnbeW, i64* %lnbeX, !tbaa !1
  %lnbf0 = load i64, i64* %R1_Var
  %lnbf1 = add i64 %lnbf0, 7
  %lnbf2 = inttoptr i64 %lnbf1 to i64*
  %lnbf3 = load i64, i64* %lnbf2, !tbaa !3
  store i64 %lnbf3, i64* %lsb16
  %lnbf4 = getelementptr inbounds i64, i64* %Sp_Arg, i32 2
  %lnbf5 = bitcast i64* %lnbf4 to i64*
  %lnbf6 = load i64, i64* %lnbf5, !tbaa !1
  store i64 %lnbf6, i64* %R1_Var
  %lnbf7 = load i64, i64* %lsb16
  %lnbf8 = getelementptr inbounds i64, i64* %Sp_Arg, i32 2
  store i64 %lnbf7, i64* %lnbf8, !tbaa !1
  %lnbf9 = load i64, i64* %R1_Var
  %lnbfa = and i64 %lnbf9, 7
  %lnbfb = icmp ne i64 %lnbfa, 0
  br i1 %lnbfb, label %ubdP, label %cbdB
cbdB:
  %lnbfd = load i64, i64* %R1_Var
  %lnbfe = inttoptr i64 %lnbfd to i64*
  %lnbff = load i64, i64* %lnbfe, !tbaa !3
  %lnbfg = inttoptr i64 %lnbff to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbfh = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbfg( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbfh, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubdP:
  %lnbfi = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbdA_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbfj = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbfi( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbfj, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbdA_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbdA_info$def to i8*)
define internal ghccc void @cbdA_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 194, i64 32}>
{
cbdA:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb18 = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnbfk = load i64*, i64** %Hp_Var
  %lnbfl = getelementptr inbounds i64, i64* %lnbfk, i32 4
  %lnbfm = ptrtoint i64* %lnbfl to i64
  %lnbfn = inttoptr i64 %lnbfm to i64*
  store i64* %lnbfn, i64** %Hp_Var
  %lnbfo = load i64*, i64** %Hp_Var
  %lnbfp = ptrtoint i64* %lnbfo to i64
  %lnbfq = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnbfr = bitcast i64* %lnbfq to i64*
  %lnbfs = load i64, i64* %lnbfr, !tbaa !4
  %lnbft = icmp ugt i64 %lnbfp, %lnbfs
  br i1 %lnbft, label %cbdM, label %cbdL
cbdL:
  %lnbfw = load i64, i64* %R1_Var
  %lnbfx = add i64 %lnbfw, 7
  %lnbfy = inttoptr i64 %lnbfx to i64*
  %lnbfz = load i64, i64* %lnbfy, !tbaa !3
  store i64 %lnbfz, i64* %lsb18
  %lnbfB = ptrtoint i8* @Disk_Stats_con_info to i64
  %lnbfA = load i64*, i64** %Hp_Var
  %lnbfC = getelementptr inbounds i64, i64* %lnbfA, i32 -3
  store i64 %lnbfB, i64* %lnbfC, !tbaa !2
  %lnbfE = load i64*, i64** %Sp_Var
  %lnbfF = getelementptr inbounds i64, i64* %lnbfE, i32 1
  %lnbfG = bitcast i64* %lnbfF to i64*
  %lnbfH = load i64, i64* %lnbfG, !tbaa !1
  %lnbfD = load i64*, i64** %Hp_Var
  %lnbfI = getelementptr inbounds i64, i64* %lnbfD, i32 -2
  store i64 %lnbfH, i64* %lnbfI, !tbaa !2
  %lnbfK = load i64*, i64** %Sp_Var
  %lnbfL = getelementptr inbounds i64, i64* %lnbfK, i32 2
  %lnbfM = bitcast i64* %lnbfL to i64*
  %lnbfN = load i64, i64* %lnbfM, !tbaa !1
  %lnbfJ = load i64*, i64** %Hp_Var
  %lnbfO = getelementptr inbounds i64, i64* %lnbfJ, i32 -1
  store i64 %lnbfN, i64* %lnbfO, !tbaa !2
  %lnbfQ = load i64, i64* %lsb18
  %lnbfP = load i64*, i64** %Hp_Var
  %lnbfR = getelementptr inbounds i64, i64* %lnbfP, i32 0
  store i64 %lnbfQ, i64* %lnbfR, !tbaa !2
  %lnbfT = load i64*, i64** %Hp_Var
  %lnbfU = ptrtoint i64* %lnbfT to i64
  %lnbfV = add i64 %lnbfU, -23
  store i64 %lnbfV, i64* %R1_Var
  %lnbfW = load i64*, i64** %Sp_Var
  %lnbfX = getelementptr inbounds i64, i64* %lnbfW, i32 3
  %lnbfY = ptrtoint i64* %lnbfX to i64
  %lnbfZ = inttoptr i64 %lnbfY to i64*
  store i64* %lnbfZ, i64** %Sp_Var
  %lnbg0 = load i64*, i64** %Sp_Var
  %lnbg1 = getelementptr inbounds i64, i64* %lnbg0, i32 0
  %lnbg2 = bitcast i64* %lnbg1 to i64*
  %lnbg3 = load i64, i64* %lnbg2, !tbaa !1
  %lnbg4 = inttoptr i64 %lnbg3 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbg5 = load i64*, i64** %Sp_Var
  %lnbg6 = load i64*, i64** %Hp_Var
  %lnbg7 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbg4( i64* %Base_Arg, i64* %lnbg5, i64* %lnbg6, i64 %lnbg7, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbdM:
  %lnbg8 = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 32, i64* %lnbg8, !tbaa !4
  %lnbg9 = load i64, i64* %R1_Var
  store i64 %lnbg9, i64* %R1_Var
  %lnbga = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbgb = load i64*, i64** %Sp_Var
  %lnbgc = load i64*, i64** %Hp_Var
  %lnbgd = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbga( i64* %Base_Arg, i64* %lnbgb, i64* %lnbgc, i64 %lnbgd, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_zdWS_closure_struct = type <{i64}>
@Disk_zdWS_closure$def = internal global %Disk_zdWS_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdWS_info$def to i64)}>
@Disk_zdWS_closure = alias i8* bitcast (%Disk_zdWS_closure_struct* @Disk_zdWS_closure$def to i8*)
@Disk_zdWS_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdWS_info$def to i8*)
define ghccc void @Disk_zdWS_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 17179869208, i64 0, i64 15}>
{
cbgy:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R5_Var = alloca i64, i32 1
  store i64 %R5_Arg, i64* %R5_Var
  %R4_Var = alloca i64, i32 1
  store i64 %R4_Arg, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnbh3 = load i64*, i64** %Sp_Var
  %lnbh4 = getelementptr inbounds i64, i64* %lnbh3, i32 -4
  %lnbh5 = ptrtoint i64* %lnbh4 to i64
  %lnbh6 = icmp ult i64 %lnbh5, %SpLim_Arg
  br i1 %lnbh6, label %cbgR, label %cbgS
cbgS:
  %lnbh8 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbgv_info$def to i64
  %lnbh7 = load i64*, i64** %Sp_Var
  %lnbh9 = getelementptr inbounds i64, i64* %lnbh7, i32 -4
  store i64 %lnbh8, i64* %lnbh9, !tbaa !1
  %lnbha = load i64, i64* %R2_Var
  store i64 %lnbha, i64* %R1_Var
  %lnbhc = load i64, i64* %R3_Var
  %lnbhb = load i64*, i64** %Sp_Var
  %lnbhd = getelementptr inbounds i64, i64* %lnbhb, i32 -3
  store i64 %lnbhc, i64* %lnbhd, !tbaa !1
  %lnbhf = load i64, i64* %R4_Var
  %lnbhe = load i64*, i64** %Sp_Var
  %lnbhg = getelementptr inbounds i64, i64* %lnbhe, i32 -2
  store i64 %lnbhf, i64* %lnbhg, !tbaa !1
  %lnbhi = load i64, i64* %R5_Var
  %lnbhh = load i64*, i64** %Sp_Var
  %lnbhj = getelementptr inbounds i64, i64* %lnbhh, i32 -1
  store i64 %lnbhi, i64* %lnbhj, !tbaa !1
  %lnbhk = load i64*, i64** %Sp_Var
  %lnbhl = getelementptr inbounds i64, i64* %lnbhk, i32 -4
  %lnbhm = ptrtoint i64* %lnbhl to i64
  %lnbhn = inttoptr i64 %lnbhm to i64*
  store i64* %lnbhn, i64** %Sp_Var
  %lnbho = load i64, i64* %R1_Var
  %lnbhp = and i64 %lnbho, 7
  %lnbhq = icmp ne i64 %lnbhp, 0
  br i1 %lnbhq, label %ubh0, label %cbgw
cbgw:
  %lnbhs = load i64, i64* %R1_Var
  %lnbht = inttoptr i64 %lnbhs to i64*
  %lnbhu = load i64, i64* %lnbht, !tbaa !3
  %lnbhv = inttoptr i64 %lnbhu to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbhw = load i64*, i64** %Sp_Var
  %lnbhx = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbhv( i64* %Base_Arg, i64* %lnbhw, i64* %Hp_Arg, i64 %lnbhx, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubh0:
  %lnbhy = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbgv_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbhz = load i64*, i64** %Sp_Var
  %lnbhA = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbhy( i64* %Base_Arg, i64* %lnbhz, i64* %Hp_Arg, i64 %lnbhA, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbgR:
  %lnbhB = load i64, i64* %R5_Var
  store i64 %lnbhB, i64* %R5_Var
  %lnbhC = load i64, i64* %R4_Var
  store i64 %lnbhC, i64* %R4_Var
  %lnbhD = load i64, i64* %R3_Var
  store i64 %lnbhD, i64* %R3_Var
  %lnbhE = load i64, i64* %R2_Var
  store i64 %lnbhE, i64* %R2_Var
  %lnbhF = ptrtoint %Disk_zdWS_closure_struct* @Disk_zdWS_closure$def to i64
  store i64 %lnbhF, i64* %R1_Var
  %lnbhG = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lnbhH = bitcast i64* %lnbhG to i64*
  %lnbhI = load i64, i64* %lnbhH, !tbaa !4
  %lnbhJ = inttoptr i64 %lnbhI to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbhK = load i64*, i64** %Sp_Var
  %lnbhL = load i64, i64* %R1_Var
  %lnbhM = load i64, i64* %R2_Var
  %lnbhN = load i64, i64* %R3_Var
  %lnbhO = load i64, i64* %R4_Var
  %lnbhP = load i64, i64* %R5_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbhJ( i64* %Base_Arg, i64* %lnbhK, i64* %Hp_Arg, i64 %lnbhL, i64 %lnbhM, i64 %lnbhN, i64 %lnbhO, i64 %lnbhP, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbgv_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbgv_info$def to i8*)
define internal ghccc void @cbgv_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 3, i64 32}>
{
cbgv:
  %lsb1e = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbhQ = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbgB_info$def to i64
  %lnbhR = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lnbhQ, i64* %lnbhR, !tbaa !1
  %lnbhU = load i64, i64* %R1_Var
  %lnbhV = add i64 %lnbhU, 7
  %lnbhW = inttoptr i64 %lnbhV to i64*
  %lnbhX = load i64, i64* %lnbhW, !tbaa !3
  store i64 %lnbhX, i64* %lsb1e
  %lnbhY = getelementptr inbounds i64, i64* %Sp_Arg, i32 1
  %lnbhZ = bitcast i64* %lnbhY to i64*
  %lnbi0 = load i64, i64* %lnbhZ, !tbaa !1
  store i64 %lnbi0, i64* %R1_Var
  %lnbi1 = load i64, i64* %lsb1e
  %lnbi2 = getelementptr inbounds i64, i64* %Sp_Arg, i32 1
  store i64 %lnbi1, i64* %lnbi2, !tbaa !1
  %lnbi3 = load i64, i64* %R1_Var
  %lnbi4 = and i64 %lnbi3, 7
  %lnbi5 = icmp ne i64 %lnbi4, 0
  br i1 %lnbi5, label %ubgZ, label %cbgC
cbgC:
  %lnbi7 = load i64, i64* %R1_Var
  %lnbi8 = inttoptr i64 %lnbi7 to i64*
  %lnbi9 = load i64, i64* %lnbi8, !tbaa !3
  %lnbia = inttoptr i64 %lnbi9 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbib = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbia( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbib, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubgZ:
  %lnbic = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbgB_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbid = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbic( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbid, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbgB_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbgB_info$def to i8*)
define internal ghccc void @cbgB_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 67, i64 32}>
{
cbgB:
  %lsb1g = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbie = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbgG_info$def to i64
  %lnbif = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lnbie, i64* %lnbif, !tbaa !1
  %lnbii = load i64, i64* %R1_Var
  %lnbij = add i64 %lnbii, 7
  %lnbik = inttoptr i64 %lnbij to i64*
  %lnbil = load i64, i64* %lnbik, !tbaa !3
  store i64 %lnbil, i64* %lsb1g
  %lnbim = getelementptr inbounds i64, i64* %Sp_Arg, i32 2
  %lnbin = bitcast i64* %lnbim to i64*
  %lnbio = load i64, i64* %lnbin, !tbaa !1
  store i64 %lnbio, i64* %R1_Var
  %lnbip = load i64, i64* %lsb1g
  %lnbiq = getelementptr inbounds i64, i64* %Sp_Arg, i32 2
  store i64 %lnbip, i64* %lnbiq, !tbaa !1
  %lnbir = load i64, i64* %R1_Var
  %lnbis = and i64 %lnbir, 7
  %lnbit = icmp ne i64 %lnbis, 0
  br i1 %lnbit, label %ubh1, label %cbgH
cbgH:
  %lnbiv = load i64, i64* %R1_Var
  %lnbiw = inttoptr i64 %lnbiv to i64*
  %lnbix = load i64, i64* %lnbiw, !tbaa !3
  %lnbiy = inttoptr i64 %lnbix to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbiz = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbiy( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbiz, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubh1:
  %lnbiA = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbgG_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbiB = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbiA( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbiB, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbgG_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbgG_info$def to i8*)
define internal ghccc void @cbgG_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 67, i64 32}>
{
cbgG:
  %lsb1h = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbiC = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbgL_info$def to i64
  %lnbiD = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lnbiC, i64* %lnbiD, !tbaa !1
  %lnbiE = load i64, i64* %R1_Var
  store i64 %lnbiE, i64* %lsb1h
  %lnbiF = getelementptr inbounds i64, i64* %Sp_Arg, i32 3
  %lnbiG = bitcast i64* %lnbiF to i64*
  %lnbiH = load i64, i64* %lnbiG, !tbaa !1
  store i64 %lnbiH, i64* %R1_Var
  %lnbiI = load i64, i64* %lsb1h
  %lnbiJ = getelementptr inbounds i64, i64* %Sp_Arg, i32 3
  store i64 %lnbiI, i64* %lnbiJ, !tbaa !1
  %lnbiK = load i64, i64* %R1_Var
  %lnbiL = and i64 %lnbiK, 7
  %lnbiM = icmp ne i64 %lnbiL, 0
  br i1 %lnbiM, label %ubh2, label %cbgM
cbgM:
  %lnbiO = load i64, i64* %R1_Var
  %lnbiP = inttoptr i64 %lnbiO to i64*
  %lnbiQ = load i64, i64* %lnbiP, !tbaa !3
  %lnbiR = inttoptr i64 %lnbiQ to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbiS = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbiR( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbiS, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubh2:
  %lnbiT = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbgL_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbiU = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbiT( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbiU, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbgL_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbgL_info$def to i8*)
define internal ghccc void @cbgL_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 67, i64 32}>
{
cbgL:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb1j = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnbiV = load i64*, i64** %Hp_Var
  %lnbiW = getelementptr inbounds i64, i64* %lnbiV, i32 5
  %lnbiX = ptrtoint i64* %lnbiW to i64
  %lnbiY = inttoptr i64 %lnbiX to i64*
  store i64* %lnbiY, i64** %Hp_Var
  %lnbiZ = load i64*, i64** %Hp_Var
  %lnbj0 = ptrtoint i64* %lnbiZ to i64
  %lnbj1 = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnbj2 = bitcast i64* %lnbj1 to i64*
  %lnbj3 = load i64, i64* %lnbj2, !tbaa !4
  %lnbj4 = icmp ugt i64 %lnbj0, %lnbj3
  br i1 %lnbj4, label %cbgY, label %cbgX
cbgX:
  %lnbj7 = load i64, i64* %R1_Var
  %lnbj8 = add i64 %lnbj7, 7
  %lnbj9 = inttoptr i64 %lnbj8 to i64*
  %lnbja = load i64, i64* %lnbj9, !tbaa !3
  store i64 %lnbja, i64* %lsb1j
  %lnbjc = ptrtoint i8* @Disk_S_con_info to i64
  %lnbjb = load i64*, i64** %Hp_Var
  %lnbjd = getelementptr inbounds i64, i64* %lnbjb, i32 -4
  store i64 %lnbjc, i64* %lnbjd, !tbaa !2
  %lnbjf = load i64*, i64** %Sp_Var
  %lnbjg = getelementptr inbounds i64, i64* %lnbjf, i32 2
  %lnbjh = bitcast i64* %lnbjg to i64*
  %lnbji = load i64, i64* %lnbjh, !tbaa !1
  %lnbje = load i64*, i64** %Hp_Var
  %lnbjj = getelementptr inbounds i64, i64* %lnbje, i32 -3
  store i64 %lnbji, i64* %lnbjj, !tbaa !2
  %lnbjl = load i64*, i64** %Sp_Var
  %lnbjm = getelementptr inbounds i64, i64* %lnbjl, i32 3
  %lnbjn = bitcast i64* %lnbjm to i64*
  %lnbjo = load i64, i64* %lnbjn, !tbaa !1
  %lnbjk = load i64*, i64** %Hp_Var
  %lnbjp = getelementptr inbounds i64, i64* %lnbjk, i32 -2
  store i64 %lnbjo, i64* %lnbjp, !tbaa !2
  %lnbjr = load i64, i64* %lsb1j
  %lnbjq = load i64*, i64** %Hp_Var
  %lnbjs = getelementptr inbounds i64, i64* %lnbjq, i32 -1
  store i64 %lnbjr, i64* %lnbjs, !tbaa !2
  %lnbju = load i64*, i64** %Sp_Var
  %lnbjv = getelementptr inbounds i64, i64* %lnbju, i32 1
  %lnbjw = bitcast i64* %lnbjv to i64*
  %lnbjx = load i64, i64* %lnbjw, !tbaa !1
  %lnbjt = load i64*, i64** %Hp_Var
  %lnbjy = getelementptr inbounds i64, i64* %lnbjt, i32 0
  store i64 %lnbjx, i64* %lnbjy, !tbaa !2
  %lnbjA = load i64*, i64** %Hp_Var
  %lnbjB = ptrtoint i64* %lnbjA to i64
  %lnbjC = add i64 %lnbjB, -31
  store i64 %lnbjC, i64* %R1_Var
  %lnbjD = load i64*, i64** %Sp_Var
  %lnbjE = getelementptr inbounds i64, i64* %lnbjD, i32 4
  %lnbjF = ptrtoint i64* %lnbjE to i64
  %lnbjG = inttoptr i64 %lnbjF to i64*
  store i64* %lnbjG, i64** %Sp_Var
  %lnbjH = load i64*, i64** %Sp_Var
  %lnbjI = getelementptr inbounds i64, i64* %lnbjH, i32 0
  %lnbjJ = bitcast i64* %lnbjI to i64*
  %lnbjK = load i64, i64* %lnbjJ, !tbaa !1
  %lnbjL = inttoptr i64 %lnbjK to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbjM = load i64*, i64** %Sp_Var
  %lnbjN = load i64*, i64** %Hp_Var
  %lnbjO = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbjL( i64* %Base_Arg, i64* %lnbjM, i64* %lnbjN, i64 %lnbjO, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbgY:
  %lnbjP = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 40, i64* %lnbjP, !tbaa !4
  %lnbjQ = load i64, i64* %R1_Var
  store i64 %lnbjQ, i64* %R1_Var
  %lnbjR = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbjS = load i64*, i64** %Sp_Var
  %lnbjT = load i64*, i64** %Hp_Var
  %lnbjU = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbjR( i64* %Base_Arg, i64* %lnbjS, i64* %lnbjT, i64 %lnbjU, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_zdsdeletezuzdsgo10_closure_struct = type <{i64, i64}>
@Disk_zdsdeletezuzdsgo10_closure$def = internal global %Disk_zdsdeletezuzdsgo10_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdsdeletezuzdsgo10_info$def to i64), i64 0}>
@Disk_zdsdeletezuzdsgo10_closure = alias i8* bitcast (%Disk_zdsdeletezuzdsgo10_closure_struct* @Disk_zdsdeletezuzdsgo10_closure$def to i8*)
@Disk_zdsdeletezuzdsgo10_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdsdeletezuzdsgo10_info$def to i8*)
define ghccc void @Disk_zdsdeletezuzdsgo10_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdsdeletezuzdsgo10_info$def to i64)),i64 0), i64 8589934607, i64 0, i64 64424509455}>
{
cbkn:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnbl9 = load i64*, i64** %Sp_Var
  %lnbla = getelementptr inbounds i64, i64* %lnbl9, i32 -6
  %lnblb = ptrtoint i64* %lnbla to i64
  %lnblc = icmp ult i64 %lnblb, %SpLim_Arg
  br i1 %lnblc, label %cbko, label %cbkp
cbkp:
  %lnble = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbkk_info$def to i64
  %lnbld = load i64*, i64** %Sp_Var
  %lnblf = getelementptr inbounds i64, i64* %lnbld, i32 -2
  store i64 %lnble, i64* %lnblf, !tbaa !1
  %lnblg = load i64, i64* %R2_Var
  store i64 %lnblg, i64* %R1_Var
  %lnbli = load i64, i64* %R3_Var
  %lnblh = load i64*, i64** %Sp_Var
  %lnblj = getelementptr inbounds i64, i64* %lnblh, i32 -1
  store i64 %lnbli, i64* %lnblj, !tbaa !1
  %lnblk = load i64*, i64** %Sp_Var
  %lnbll = getelementptr inbounds i64, i64* %lnblk, i32 -2
  %lnblm = ptrtoint i64* %lnbll to i64
  %lnbln = inttoptr i64 %lnblm to i64*
  store i64* %lnbln, i64** %Sp_Var
  %lnblo = load i64, i64* %R1_Var
  %lnblp = and i64 %lnblo, 7
  %lnblq = icmp ne i64 %lnblp, 0
  br i1 %lnblq, label %ubl7, label %cbkl
cbkl:
  %lnbls = load i64, i64* %R1_Var
  %lnblt = inttoptr i64 %lnbls to i64*
  %lnblu = load i64, i64* %lnblt, !tbaa !3
  %lnblv = inttoptr i64 %lnblu to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnblw = load i64*, i64** %Sp_Var
  %lnblx = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnblv( i64* %Base_Arg, i64* %lnblw, i64* %Hp_Arg, i64 %lnblx, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubl7:
  %lnbly = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbkk_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnblz = load i64*, i64** %Sp_Var
  %lnblA = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbly( i64* %Base_Arg, i64* %lnblz, i64* %Hp_Arg, i64 %lnblA, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbko:
  %lnblB = load i64, i64* %R3_Var
  store i64 %lnblB, i64* %R3_Var
  %lnblC = load i64, i64* %R2_Var
  store i64 %lnblC, i64* %R2_Var
  %lnblD = ptrtoint %Disk_zdsdeletezuzdsgo10_closure_struct* @Disk_zdsdeletezuzdsgo10_closure$def to i64
  store i64 %lnblD, i64* %R1_Var
  %lnblE = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lnblF = bitcast i64* %lnblE to i64*
  %lnblG = load i64, i64* %lnblF, !tbaa !4
  %lnblH = inttoptr i64 %lnblG to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnblI = load i64*, i64** %Sp_Var
  %lnblJ = load i64, i64* %R1_Var
  %lnblK = load i64, i64* %R2_Var
  %lnblL = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnblH( i64* %Base_Arg, i64* %lnblI, i64* %Hp_Arg, i64 %lnblJ, i64 %lnblK, i64 %lnblL, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbkk_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbkk_info$def to i8*)
define internal ghccc void @cbkk_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbkk_info$def to i64)),i64 0), i64 1, i64 64424509472}>
{
cbkk:
  %lsb1m = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnblM = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbks_info$def to i64
  %lnblN = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lnblM, i64* %lnblN, !tbaa !1
  %lnblO = load i64, i64* %R1_Var
  store i64 %lnblO, i64* %lsb1m
  %lnblP = getelementptr inbounds i64, i64* %Sp_Arg, i32 1
  %lnblQ = bitcast i64* %lnblP to i64*
  %lnblR = load i64, i64* %lnblQ, !tbaa !1
  store i64 %lnblR, i64* %R1_Var
  %lnblS = load i64, i64* %lsb1m
  %lnblT = getelementptr inbounds i64, i64* %Sp_Arg, i32 1
  store i64 %lnblS, i64* %lnblT, !tbaa !1
  %lnblU = load i64, i64* %R1_Var
  %lnblV = and i64 %lnblU, 7
  %lnblW = icmp ne i64 %lnblV, 0
  br i1 %lnblW, label %ubl6, label %cbku
cbku:
  %lnblY = load i64, i64* %R1_Var
  %lnblZ = inttoptr i64 %lnblY to i64*
  %lnbm0 = load i64, i64* %lnblZ, !tbaa !3
  %lnbm1 = inttoptr i64 %lnbm0 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbm2 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbm1( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbm2, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubl6:
  %lnbm3 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbks_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbm4 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbm3( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbm4, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbks_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbks_info$def to i8*)
define internal ghccc void @cbks_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbks_info$def to i64)),i64 0), i64 1, i64 64424509472}>
{
cbks:
  %lsb1p = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbm5 = load i64, i64* %R1_Var
  %lnbm6 = and i64 %lnbm5, 7
  switch i64 %lnbm6, label %cbkB [i64 1, label %cbkB
i64 2, label %cbl3]
cbkB:
  %lnbm8 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbky_info$def to i64
  %lnbm7 = load i64*, i64** %Sp_Var
  %lnbm9 = getelementptr inbounds i64, i64* %lnbm7, i32 -4
  store i64 %lnbm8, i64* %lnbm9, !tbaa !1
  %lnbmc = load i64, i64* %R1_Var
  %lnbmd = add i64 %lnbmc, 7
  %lnbme = inttoptr i64 %lnbmd to i64*
  %lnbmf = load i64, i64* %lnbme, !tbaa !3
  store i64 %lnbmf, i64* %lsb1p
  %lnbmg = load i64, i64* %lsb1p
  store i64 %lnbmg, i64* %R3_Var
  %lnbmh = load i64*, i64** %Sp_Var
  %lnbmi = getelementptr inbounds i64, i64* %lnbmh, i32 1
  %lnbmj = bitcast i64* %lnbmi to i64*
  %lnbmk = load i64, i64* %lnbmj, !tbaa !1
  store i64 %lnbmk, i64* %R2_Var
  %lnbmo = load i64, i64* %R1_Var
  %lnbmp = add i64 %lnbmo, 15
  %lnbmq = inttoptr i64 %lnbmp to i64*
  %lnbmr = load i64, i64* %lnbmq, !tbaa !3
  %lnbml = load i64*, i64** %Sp_Var
  %lnbms = getelementptr inbounds i64, i64* %lnbml, i32 -3
  store i64 %lnbmr, i64* %lnbms, !tbaa !1
  %lnbmw = load i64, i64* %R1_Var
  %lnbmx = add i64 %lnbmw, 23
  %lnbmy = inttoptr i64 %lnbmx to i64*
  %lnbmz = load i64, i64* %lnbmy, !tbaa !3
  %lnbmt = load i64*, i64** %Sp_Var
  %lnbmA = getelementptr inbounds i64, i64* %lnbmt, i32 -2
  store i64 %lnbmz, i64* %lnbmA, !tbaa !1
  %lnbmE = load i64, i64* %R1_Var
  %lnbmF = add i64 %lnbmE, 31
  %lnbmG = inttoptr i64 %lnbmF to i64*
  %lnbmH = load i64, i64* %lnbmG, !tbaa !3
  %lnbmB = load i64*, i64** %Sp_Var
  %lnbmI = getelementptr inbounds i64, i64* %lnbmB, i32 -1
  store i64 %lnbmH, i64* %lnbmI, !tbaa !1
  %lnbmK = load i64, i64* %lsb1p
  %lnbmJ = load i64*, i64** %Sp_Var
  %lnbmL = getelementptr inbounds i64, i64* %lnbmJ, i32 0
  store i64 %lnbmK, i64* %lnbmL, !tbaa !1
  %lnbmM = load i64*, i64** %Sp_Var
  %lnbmN = getelementptr inbounds i64, i64* %lnbmM, i32 -4
  %lnbmO = ptrtoint i64* %lnbmN to i64
  %lnbmP = inttoptr i64 %lnbmO to i64*
  store i64* %lnbmP, i64** %Sp_Var
  %lnbmQ = bitcast i8* @integerzmgmp_GHCziIntegerziType_compareInteger_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbmR = load i64*, i64** %Sp_Var
  %lnbmS = load i64, i64* %R1_Var
  %lnbmT = load i64, i64* %R2_Var
  %lnbmU = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbmQ( i64* %Base_Arg, i64* %lnbmR, i64* %Hp_Arg, i64 %lnbmS, i64 %lnbmT, i64 %lnbmU, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbl3:
  %lnbmV = ptrtoint i8* @containerszm0zi5zi7zi1_DataziMapziBase_Tip_closure to i64
  %lnbmW = add i64 %lnbmV, 2
  store i64 %lnbmW, i64* %R1_Var
  %lnbmX = load i64*, i64** %Sp_Var
  %lnbmY = getelementptr inbounds i64, i64* %lnbmX, i32 2
  %lnbmZ = ptrtoint i64* %lnbmY to i64
  %lnbn0 = inttoptr i64 %lnbmZ to i64*
  store i64* %lnbn0, i64** %Sp_Var
  %lnbn1 = load i64*, i64** %Sp_Var
  %lnbn2 = getelementptr inbounds i64, i64* %lnbn1, i32 0
  %lnbn3 = bitcast i64* %lnbn2 to i64*
  %lnbn4 = load i64, i64* %lnbn3, !tbaa !1
  %lnbn5 = inttoptr i64 %lnbn4 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbn6 = load i64*, i64** %Sp_Var
  %lnbn7 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbn5( i64* %Base_Arg, i64* %lnbn6, i64* %Hp_Arg, i64 %lnbn7, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbky_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbky_info$def to i8*)
define internal ghccc void @cbky_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbky_info$def to i64)),i64 0), i64 5, i64 64424509472}>
{
cbky:
  %lsb1m = alloca i64, i32 1
  %lsb1r = alloca i64, i32 1
  %lsb1s = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnbn8 = load i64*, i64** %Sp_Var
  %lnbn9 = getelementptr inbounds i64, i64* %lnbn8, i32 5
  %lnbna = bitcast i64* %lnbn9 to i64*
  %lnbnb = load i64, i64* %lnbna, !tbaa !1
  store i64 %lnbnb, i64* %lsb1m
  %lnbnc = load i64*, i64** %Sp_Var
  %lnbnd = getelementptr inbounds i64, i64* %lnbnc, i32 2
  %lnbne = bitcast i64* %lnbnd to i64*
  %lnbnf = load i64, i64* %lnbne, !tbaa !1
  store i64 %lnbnf, i64* %lsb1r
  %lnbng = load i64*, i64** %Sp_Var
  %lnbnh = getelementptr inbounds i64, i64* %lnbng, i32 3
  %lnbni = bitcast i64* %lnbnh to i64*
  %lnbnj = load i64, i64* %lnbni, !tbaa !1
  store i64 %lnbnj, i64* %lsb1s
  %lnbnk = and i64 %R1_Arg, 7
  switch i64 %lnbnk, label %cbkI [i64 1, label %cbkI
i64 2, label %cbkQ
i64 3, label %cbkV]
cbkI:
  %lnbnm = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbkF_info$def to i64
  %lnbnl = load i64*, i64** %Sp_Var
  %lnbnn = getelementptr inbounds i64, i64* %lnbnl, i32 0
  store i64 %lnbnm, i64* %lnbnn, !tbaa !1
  %lnbno = load i64, i64* %lsb1r
  store i64 %lnbno, i64* %R3_Var
  %lnbnp = load i64, i64* %lsb1m
  store i64 %lnbnp, i64* %R2_Var
  %lnbnq = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdsdeletezuzdsgo10_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbnr = load i64*, i64** %Sp_Var
  %lnbns = load i64, i64* %R2_Var
  %lnbnt = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbnq( i64* %Base_Arg, i64* %lnbnr, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnbns, i64 %lnbnt, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbkQ:
  %lnbnu = load i64, i64* %lsb1s
  store i64 %lnbnu, i64* %R3_Var
  %lnbnv = load i64, i64* %lsb1r
  store i64 %lnbnv, i64* %R2_Var
  %lnbnw = load i64*, i64** %Sp_Var
  %lnbnx = getelementptr inbounds i64, i64* %lnbnw, i32 6
  %lnbny = ptrtoint i64* %lnbnx to i64
  %lnbnz = inttoptr i64 %lnbny to i64*
  store i64* %lnbnz, i64** %Sp_Var
  %lnbnA = bitcast i8* @containerszm0zi5zi7zi1_DataziMapziBase_glue_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbnB = load i64*, i64** %Sp_Var
  %lnbnC = load i64, i64* %R2_Var
  %lnbnD = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbnA( i64* %Base_Arg, i64* %lnbnB, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnbnC, i64 %lnbnD, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbkV:
  %lnbnF = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbkT_info$def to i64
  %lnbnE = load i64*, i64** %Sp_Var
  %lnbnG = getelementptr inbounds i64, i64* %lnbnE, i32 0
  store i64 %lnbnF, i64* %lnbnG, !tbaa !1
  %lnbnH = load i64, i64* %lsb1s
  store i64 %lnbnH, i64* %R3_Var
  %lnbnI = load i64, i64* %lsb1m
  store i64 %lnbnI, i64* %R2_Var
  %lnbnJ = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdsdeletezuzdsgo10_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbnK = load i64*, i64** %Sp_Var
  %lnbnL = load i64, i64* %R2_Var
  %lnbnM = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbnJ( i64* %Base_Arg, i64* %lnbnK, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnbnL, i64 %lnbnM, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbkT_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbkT_info$def to i8*)
define internal ghccc void @cbkT_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbkT_info$def to i64)),i64 16), i64 1285, i64 4294967328}>
{
cbkT:
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  store i64 %R1_Arg, i64* %R5_Var
  %lnbnN = load i64*, i64** %Sp_Var
  %lnbnO = getelementptr inbounds i64, i64* %lnbnN, i32 2
  %lnbnP = bitcast i64* %lnbnO to i64*
  %lnbnQ = load i64, i64* %lnbnP, !tbaa !1
  store i64 %lnbnQ, i64* %R4_Var
  %lnbnR = load i64*, i64** %Sp_Var
  %lnbnS = getelementptr inbounds i64, i64* %lnbnR, i32 1
  %lnbnT = bitcast i64* %lnbnS to i64*
  %lnbnU = load i64, i64* %lnbnT, !tbaa !1
  store i64 %lnbnU, i64* %R3_Var
  %lnbnV = load i64*, i64** %Sp_Var
  %lnbnW = getelementptr inbounds i64, i64* %lnbnV, i32 4
  %lnbnX = bitcast i64* %lnbnW to i64*
  %lnbnY = load i64, i64* %lnbnX, !tbaa !1
  store i64 %lnbnY, i64* %R2_Var
  %lnbnZ = load i64*, i64** %Sp_Var
  %lnbo0 = getelementptr inbounds i64, i64* %lnbnZ, i32 6
  %lnbo1 = ptrtoint i64* %lnbo0 to i64
  %lnbo2 = inttoptr i64 %lnbo1 to i64*
  store i64* %lnbo2, i64** %Sp_Var
  %lnbo3 = bitcast i8* @containerszm0zi5zi7zi1_DataziMapziBase_balanceL_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbo4 = load i64*, i64** %Sp_Var
  %lnbo5 = load i64, i64* %R2_Var
  %lnbo6 = load i64, i64* %R3_Var
  %lnbo7 = load i64, i64* %R4_Var
  %lnbo8 = load i64, i64* %R5_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbo3( i64* %Base_Arg, i64* %lnbo4, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnbo5, i64 %lnbo6, i64 %lnbo7, i64 %lnbo8, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbkF_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbkF_info$def to i8*)
define internal ghccc void @cbkF_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbkF_info$def to i64)),i64 8), i64 1157, i64 4294967328}>
{
cbkF:
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnbo9 = load i64*, i64** %Sp_Var
  %lnboa = getelementptr inbounds i64, i64* %lnbo9, i32 3
  %lnbob = bitcast i64* %lnboa to i64*
  %lnboc = load i64, i64* %lnbob, !tbaa !1
  store i64 %lnboc, i64* %R5_Var
  store i64 %R1_Arg, i64* %R4_Var
  %lnbod = load i64*, i64** %Sp_Var
  %lnboe = getelementptr inbounds i64, i64* %lnbod, i32 1
  %lnbof = bitcast i64* %lnboe to i64*
  %lnbog = load i64, i64* %lnbof, !tbaa !1
  store i64 %lnbog, i64* %R3_Var
  %lnboh = load i64*, i64** %Sp_Var
  %lnboi = getelementptr inbounds i64, i64* %lnboh, i32 4
  %lnboj = bitcast i64* %lnboi to i64*
  %lnbok = load i64, i64* %lnboj, !tbaa !1
  store i64 %lnbok, i64* %R2_Var
  %lnbol = load i64*, i64** %Sp_Var
  %lnbom = getelementptr inbounds i64, i64* %lnbol, i32 6
  %lnbon = ptrtoint i64* %lnbom to i64
  %lnboo = inttoptr i64 %lnbon to i64*
  store i64* %lnboo, i64** %Sp_Var
  %lnbop = bitcast i8* @containerszm0zi5zi7zi1_DataziMapziBase_balanceR_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnboq = load i64*, i64** %Sp_Var
  %lnbor = load i64, i64* %R2_Var
  %lnbos = load i64, i64* %R3_Var
  %lnbot = load i64, i64* %R4_Var
  %lnbou = load i64, i64* %R5_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbop( i64* %Base_Arg, i64* %lnboq, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnbor, i64 %lnbos, i64 %lnbot, i64 %lnbou, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_zdslookup1_closure_struct = type <{i64}>
@Disk_zdslookup1_closure$def = internal global %Disk_zdslookup1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdslookup1_info$def to i64)}>
@Disk_zdslookup1_closure = alias i8* bitcast (%Disk_zdslookup1_closure_struct* @Disk_zdslookup1_closure$def to i8*)
@Disk_zdslookup1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdslookup1_info$def to i8*)
define ghccc void @Disk_zdslookup1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934607, i64 0, i64 15}>
{
cboR:
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnbpB = load i64*, i64** %Sp_Var
  %lnbpC = getelementptr inbounds i64, i64* %lnbpB, i32 -5
  %lnbpD = ptrtoint i64* %lnbpC to i64
  %lnbpE = icmp uge i64 %lnbpD, %SpLim_Arg
  br i1 %lnbpE, label %ubpt, label %cboS
cboS:
  %lnbpF = load i64, i64* %R3_Var
  store i64 %lnbpF, i64* %R3_Var
  %lnbpG = load i64, i64* %R2_Var
  store i64 %lnbpG, i64* %R2_Var
  %lnbpH = ptrtoint %Disk_zdslookup1_closure_struct* @Disk_zdslookup1_closure$def to i64
  store i64 %lnbpH, i64* %R1_Var
  %lnbpI = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lnbpJ = bitcast i64* %lnbpI to i64*
  %lnbpK = load i64, i64* %lnbpJ, !tbaa !4
  %lnbpL = inttoptr i64 %lnbpK to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbpM = load i64*, i64** %Sp_Var
  %lnbpN = load i64, i64* %R1_Var
  %lnbpO = load i64, i64* %R2_Var
  %lnbpP = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbpL( i64* %Base_Arg, i64* %lnbpM, i64* %Hp_Arg, i64 %lnbpN, i64 %lnbpO, i64 %lnbpP, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubpt:
  %lnbpR = load i64, i64* %R2_Var
  %lnbpQ = load i64*, i64** %Sp_Var
  %lnbpS = getelementptr inbounds i64, i64* %lnbpQ, i32 -2
  store i64 %lnbpR, i64* %lnbpS, !tbaa !1
  %lnbpU = load i64, i64* %R3_Var
  %lnbpT = load i64*, i64** %Sp_Var
  %lnbpV = getelementptr inbounds i64, i64* %lnbpT, i32 -1
  store i64 %lnbpU, i64* %lnbpV, !tbaa !1
  %lnbpW = load i64*, i64** %Sp_Var
  %lnbpX = getelementptr inbounds i64, i64* %lnbpW, i32 -2
  %lnbpY = ptrtoint i64* %lnbpX to i64
  %lnbpZ = inttoptr i64 %lnbpY to i64*
  store i64* %lnbpZ, i64** %Sp_Var
  %lnbq0 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cboL_entry$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbq1 = load i64*, i64** %Sp_Var
  %lnbq2 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbq0( i64* %Base_Arg, i64* %lnbq1, i64* %Hp_Arg, i64 %lnbq2, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cboL_entry = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cboL_entry$def to i8*)
define internal ghccc void @cboL_entry$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind
{
cboL:
  %lsb1w = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbq3 = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  %lnbq4 = bitcast i64* %lnbq3 to i64*
  %lnbq5 = load i64, i64* %lnbq4, !tbaa !1
  store i64 %lnbq5, i64* %lsb1w
  %lnbq6 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cboO_info$def to i64
  %lnbq7 = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lnbq6, i64* %lnbq7, !tbaa !1
  %lnbq8 = load i64, i64* %lsb1w
  store i64 %lnbq8, i64* %R1_Var
  %lnbq9 = load i64, i64* %R1_Var
  %lnbqa = and i64 %lnbq9, 7
  %lnbqb = icmp ne i64 %lnbqa, 0
  br i1 %lnbqb, label %ubpw, label %cboP
cboP:
  %lnbqd = load i64, i64* %R1_Var
  %lnbqe = inttoptr i64 %lnbqd to i64*
  %lnbqf = load i64, i64* %lnbqe, !tbaa !3
  %lnbqg = inttoptr i64 %lnbqf to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbqh = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbqg( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbqh, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubpw:
  %lnbqi = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cboO_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbqj = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbqi( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbqj, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cboO_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cboO_info$def to i8*)
define internal ghccc void @cboO_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 1, i64 32}>
{
cboO:
  %lsb1y = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbqk = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cboW_info$def to i64
  %lnbql = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lnbqk, i64* %lnbql, !tbaa !1
  %lnbqm = load i64, i64* %R1_Var
  store i64 %lnbqm, i64* %lsb1y
  %lnbqn = getelementptr inbounds i64, i64* %Sp_Arg, i32 1
  %lnbqo = bitcast i64* %lnbqn to i64*
  %lnbqp = load i64, i64* %lnbqo, !tbaa !1
  store i64 %lnbqp, i64* %R1_Var
  %lnbqq = load i64, i64* %lsb1y
  %lnbqr = getelementptr inbounds i64, i64* %Sp_Arg, i32 1
  store i64 %lnbqq, i64* %lnbqr, !tbaa !1
  %lnbqs = load i64, i64* %R1_Var
  %lnbqt = and i64 %lnbqs, 7
  %lnbqu = icmp ne i64 %lnbqt, 0
  br i1 %lnbqu, label %ubpx, label %cboY
cboY:
  %lnbqw = load i64, i64* %R1_Var
  %lnbqx = inttoptr i64 %lnbqw to i64*
  %lnbqy = load i64, i64* %lnbqx, !tbaa !3
  %lnbqz = inttoptr i64 %lnbqy to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbqA = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbqz( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbqA, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubpx:
  %lnbqB = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cboW_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbqC = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbqB( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbqC, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cboW_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cboW_info$def to i8*)
define internal ghccc void @cboW_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 1, i64 32}>
{
cboW:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbqD = load i64, i64* %R1_Var
  %lnbqE = and i64 %lnbqD, 7
  switch i64 %lnbqE, label %cbp5 [i64 1, label %cbp5
i64 2, label %cbpq]
cbp5:
  %lnbqG = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbp2_info$def to i64
  %lnbqF = load i64*, i64** %Sp_Var
  %lnbqH = getelementptr inbounds i64, i64* %lnbqF, i32 -3
  store i64 %lnbqG, i64* %lnbqH, !tbaa !1
  %lnbqK = load i64, i64* %R1_Var
  %lnbqL = add i64 %lnbqK, 7
  %lnbqM = inttoptr i64 %lnbqL to i64*
  %lnbqN = load i64, i64* %lnbqM, !tbaa !3
  store i64 %lnbqN, i64* %R3_Var
  %lnbqO = load i64*, i64** %Sp_Var
  %lnbqP = getelementptr inbounds i64, i64* %lnbqO, i32 1
  %lnbqQ = bitcast i64* %lnbqP to i64*
  %lnbqR = load i64, i64* %lnbqQ, !tbaa !1
  store i64 %lnbqR, i64* %R2_Var
  %lnbqV = load i64, i64* %R1_Var
  %lnbqW = add i64 %lnbqV, 23
  %lnbqX = inttoptr i64 %lnbqW to i64*
  %lnbqY = load i64, i64* %lnbqX, !tbaa !3
  %lnbqS = load i64*, i64** %Sp_Var
  %lnbqZ = getelementptr inbounds i64, i64* %lnbqS, i32 -2
  store i64 %lnbqY, i64* %lnbqZ, !tbaa !1
  %lnbr3 = load i64, i64* %R1_Var
  %lnbr4 = add i64 %lnbr3, 31
  %lnbr5 = inttoptr i64 %lnbr4 to i64*
  %lnbr6 = load i64, i64* %lnbr5, !tbaa !3
  %lnbr0 = load i64*, i64** %Sp_Var
  %lnbr7 = getelementptr inbounds i64, i64* %lnbr0, i32 -1
  store i64 %lnbr6, i64* %lnbr7, !tbaa !1
  %lnbrb = load i64, i64* %R1_Var
  %lnbrc = add i64 %lnbrb, 15
  %lnbrd = inttoptr i64 %lnbrc to i64*
  %lnbre = load i64, i64* %lnbrd, !tbaa !3
  %lnbr8 = load i64*, i64** %Sp_Var
  %lnbrf = getelementptr inbounds i64, i64* %lnbr8, i32 0
  store i64 %lnbre, i64* %lnbrf, !tbaa !1
  %lnbrg = load i64*, i64** %Sp_Var
  %lnbrh = getelementptr inbounds i64, i64* %lnbrg, i32 -3
  %lnbri = ptrtoint i64* %lnbrh to i64
  %lnbrj = inttoptr i64 %lnbri to i64*
  store i64* %lnbrj, i64** %Sp_Var
  %lnbrk = bitcast i8* @integerzmgmp_GHCziIntegerziType_compareInteger_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbrl = load i64*, i64** %Sp_Var
  %lnbrm = load i64, i64* %R1_Var
  %lnbrn = load i64, i64* %R2_Var
  %lnbro = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbrk( i64* %Base_Arg, i64* %lnbrl, i64* %Hp_Arg, i64 %lnbrm, i64 %lnbrn, i64 %lnbro, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbpq:
  %lnbrp = ptrtoint i8* @base_GHCziBase_Nothing_closure to i64
  %lnbrq = add i64 %lnbrp, 1
  store i64 %lnbrq, i64* %R1_Var
  %lnbrr = load i64*, i64** %Sp_Var
  %lnbrs = getelementptr inbounds i64, i64* %lnbrr, i32 2
  %lnbrt = ptrtoint i64* %lnbrs to i64
  %lnbru = inttoptr i64 %lnbrt to i64*
  store i64* %lnbru, i64** %Sp_Var
  %lnbrv = load i64*, i64** %Sp_Var
  %lnbrw = getelementptr inbounds i64, i64* %lnbrv, i32 0
  %lnbrx = bitcast i64* %lnbrw to i64*
  %lnbry = load i64, i64* %lnbrx, !tbaa !1
  %lnbrz = inttoptr i64 %lnbry to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbrA = load i64*, i64** %Sp_Var
  %lnbrB = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbrz( i64* %Base_Arg, i64* %lnbrA, i64* %Hp_Arg, i64 %lnbrB, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbp2_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbp2_info$def to i8*)
define internal ghccc void @cbp2_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 4, i64 32}>
{
cbp2:
  %lsb1y = alloca i64, i32 1
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbrC = load i64*, i64** %Sp_Var
  %lnbrD = getelementptr inbounds i64, i64* %lnbrC, i32 4
  %lnbrE = bitcast i64* %lnbrD to i64*
  %lnbrF = load i64, i64* %lnbrE, !tbaa !1
  store i64 %lnbrF, i64* %lsb1y
  %lnbrG = load i64, i64* %R1_Var
  %lnbrH = and i64 %lnbrG, 7
  switch i64 %lnbrH, label %cbpb [i64 1, label %cbpb
i64 2, label %cbpf
i64 3, label %cbpm]
cbpb:
  %lnbrJ = load i64, i64* %lsb1y
  %lnbrI = load i64*, i64** %Sp_Var
  %lnbrK = getelementptr inbounds i64, i64* %lnbrI, i32 3
  store i64 %lnbrJ, i64* %lnbrK, !tbaa !1
  %lnbrM = load i64*, i64** %Sp_Var
  %lnbrN = getelementptr inbounds i64, i64* %lnbrM, i32 1
  %lnbrO = bitcast i64* %lnbrN to i64*
  %lnbrP = load i64, i64* %lnbrO, !tbaa !1
  %lnbrL = load i64*, i64** %Sp_Var
  %lnbrQ = getelementptr inbounds i64, i64* %lnbrL, i32 4
  store i64 %lnbrP, i64* %lnbrQ, !tbaa !1
  %lnbrR = load i64*, i64** %Sp_Var
  %lnbrS = getelementptr inbounds i64, i64* %lnbrR, i32 3
  %lnbrT = ptrtoint i64* %lnbrS to i64
  %lnbrU = inttoptr i64 %lnbrT to i64*
  store i64* %lnbrU, i64** %Sp_Var
  br label %ubpA
ubpA:
  %lnbrV = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cboL_entry$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbrW = load i64*, i64** %Sp_Var
  %lnbrX = load i64*, i64** %Hp_Var
  %lnbrY = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbrV( i64* %Base_Arg, i64* %lnbrW, i64* %lnbrX, i64 %lnbrY, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbpf:
  %lnbrZ = load i64*, i64** %Hp_Var
  %lnbs0 = getelementptr inbounds i64, i64* %lnbrZ, i32 2
  %lnbs1 = ptrtoint i64* %lnbs0 to i64
  %lnbs2 = inttoptr i64 %lnbs1 to i64*
  store i64* %lnbs2, i64** %Hp_Var
  %lnbs3 = load i64*, i64** %Hp_Var
  %lnbs4 = ptrtoint i64* %lnbs3 to i64
  %lnbs5 = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnbs6 = bitcast i64* %lnbs5 to i64*
  %lnbs7 = load i64, i64* %lnbs6, !tbaa !4
  %lnbs8 = icmp ugt i64 %lnbs4, %lnbs7
  br i1 %lnbs8, label %cbpi, label %cbph
cbph:
  %lnbsa = ptrtoint i8* @base_GHCziBase_Just_con_info to i64
  %lnbs9 = load i64*, i64** %Hp_Var
  %lnbsb = getelementptr inbounds i64, i64* %lnbs9, i32 -1
  store i64 %lnbsa, i64* %lnbsb, !tbaa !2
  %lnbsd = load i64*, i64** %Sp_Var
  %lnbse = getelementptr inbounds i64, i64* %lnbsd, i32 3
  %lnbsf = bitcast i64* %lnbse to i64*
  %lnbsg = load i64, i64* %lnbsf, !tbaa !1
  %lnbsc = load i64*, i64** %Hp_Var
  %lnbsh = getelementptr inbounds i64, i64* %lnbsc, i32 0
  store i64 %lnbsg, i64* %lnbsh, !tbaa !2
  %lnbsj = load i64*, i64** %Hp_Var
  %lnbsk = ptrtoint i64* %lnbsj to i64
  %lnbsl = add i64 %lnbsk, -6
  store i64 %lnbsl, i64* %R1_Var
  %lnbsm = load i64*, i64** %Sp_Var
  %lnbsn = getelementptr inbounds i64, i64* %lnbsm, i32 5
  %lnbso = ptrtoint i64* %lnbsn to i64
  %lnbsp = inttoptr i64 %lnbso to i64*
  store i64* %lnbsp, i64** %Sp_Var
  %lnbsq = load i64*, i64** %Sp_Var
  %lnbsr = getelementptr inbounds i64, i64* %lnbsq, i32 0
  %lnbss = bitcast i64* %lnbsr to i64*
  %lnbst = load i64, i64* %lnbss, !tbaa !1
  %lnbsu = inttoptr i64 %lnbst to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbsv = load i64*, i64** %Sp_Var
  %lnbsw = load i64*, i64** %Hp_Var
  %lnbsx = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbsu( i64* %Base_Arg, i64* %lnbsv, i64* %lnbsw, i64 %lnbsx, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbpi:
  %lnbsy = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 16, i64* %lnbsy, !tbaa !4
  %lnbsz = load i64, i64* %R1_Var
  store i64 %lnbsz, i64* %R1_Var
  %lnbsA = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbsB = load i64*, i64** %Sp_Var
  %lnbsC = load i64*, i64** %Hp_Var
  %lnbsD = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbsA( i64* %Base_Arg, i64* %lnbsB, i64* %lnbsC, i64 %lnbsD, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbpm:
  %lnbsF = load i64, i64* %lsb1y
  %lnbsE = load i64*, i64** %Sp_Var
  %lnbsG = getelementptr inbounds i64, i64* %lnbsE, i32 3
  store i64 %lnbsF, i64* %lnbsG, !tbaa !1
  %lnbsI = load i64*, i64** %Sp_Var
  %lnbsJ = getelementptr inbounds i64, i64* %lnbsI, i32 2
  %lnbsK = bitcast i64* %lnbsJ to i64*
  %lnbsL = load i64, i64* %lnbsK, !tbaa !1
  %lnbsH = load i64*, i64** %Sp_Var
  %lnbsM = getelementptr inbounds i64, i64* %lnbsH, i32 4
  store i64 %lnbsL, i64* %lnbsM, !tbaa !1
  %lnbsN = load i64*, i64** %Sp_Var
  %lnbsO = getelementptr inbounds i64, i64* %lnbsN, i32 3
  %lnbsP = ptrtoint i64* %lnbsO to i64
  %lnbsQ = inttoptr i64 %lnbsP to i64*
  store i64* %lnbsQ, i64** %Sp_Var
  br label %ubpA
}
%Disk_zdsinsertzuzdsgo4_closure_struct = type <{i64, i64}>
@Disk_zdsinsertzuzdsgo4_closure$def = internal global %Disk_zdsinsertzuzdsgo4_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdsinsertzuzdsgo4_info$def to i64), i64 0}>
@Disk_zdsinsertzuzdsgo4_closure = alias i8* bitcast (%Disk_zdsinsertzuzdsgo4_closure_struct* @Disk_zdsinsertzuzdsgo4_closure$def to i8*)
@Disk_zdsinsertzuzdsgo4_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdsinsertzuzdsgo4_info$def to i8*)
define ghccc void @Disk_zdsinsertzuzdsgo4_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdsinsertzuzdsgo4_info$def to i64)),i64 8), i64 12884901911, i64 0, i64 47244640271}>
{
cbtm:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R4_Var = alloca i64, i32 1
  store i64 %R4_Arg, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnbug = load i64*, i64** %Sp_Var
  %lnbuh = getelementptr inbounds i64, i64* %lnbug, i32 -8
  %lnbui = ptrtoint i64* %lnbuh to i64
  %lnbuj = icmp ult i64 %lnbui, %SpLim_Arg
  br i1 %lnbuj, label %cbtn, label %cbto
cbto:
  %lnbul = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbtj_info$def to i64
  %lnbuk = load i64*, i64** %Sp_Var
  %lnbum = getelementptr inbounds i64, i64* %lnbuk, i32 -3
  store i64 %lnbul, i64* %lnbum, !tbaa !1
  %lnbun = load i64, i64* %R2_Var
  store i64 %lnbun, i64* %R1_Var
  %lnbup = load i64, i64* %R3_Var
  %lnbuo = load i64*, i64** %Sp_Var
  %lnbuq = getelementptr inbounds i64, i64* %lnbuo, i32 -2
  store i64 %lnbup, i64* %lnbuq, !tbaa !1
  %lnbus = load i64, i64* %R4_Var
  %lnbur = load i64*, i64** %Sp_Var
  %lnbut = getelementptr inbounds i64, i64* %lnbur, i32 -1
  store i64 %lnbus, i64* %lnbut, !tbaa !1
  %lnbuu = load i64*, i64** %Sp_Var
  %lnbuv = getelementptr inbounds i64, i64* %lnbuu, i32 -3
  %lnbuw = ptrtoint i64* %lnbuv to i64
  %lnbux = inttoptr i64 %lnbuw to i64*
  store i64* %lnbux, i64** %Sp_Var
  %lnbuy = load i64, i64* %R1_Var
  %lnbuz = and i64 %lnbuy, 7
  %lnbuA = icmp ne i64 %lnbuz, 0
  br i1 %lnbuA, label %ubue, label %cbtk
cbtk:
  %lnbuC = load i64, i64* %R1_Var
  %lnbuD = inttoptr i64 %lnbuC to i64*
  %lnbuE = load i64, i64* %lnbuD, !tbaa !3
  %lnbuF = inttoptr i64 %lnbuE to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbuG = load i64*, i64** %Sp_Var
  %lnbuH = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbuF( i64* %Base_Arg, i64* %lnbuG, i64* %Hp_Arg, i64 %lnbuH, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubue:
  %lnbuI = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbtj_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbuJ = load i64*, i64** %Sp_Var
  %lnbuK = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbuI( i64* %Base_Arg, i64* %lnbuJ, i64* %Hp_Arg, i64 %lnbuK, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbtn:
  %lnbuL = load i64, i64* %R4_Var
  store i64 %lnbuL, i64* %R4_Var
  %lnbuM = load i64, i64* %R3_Var
  store i64 %lnbuM, i64* %R3_Var
  %lnbuN = load i64, i64* %R2_Var
  store i64 %lnbuN, i64* %R2_Var
  %lnbuO = ptrtoint %Disk_zdsinsertzuzdsgo4_closure_struct* @Disk_zdsinsertzuzdsgo4_closure$def to i64
  store i64 %lnbuO, i64* %R1_Var
  %lnbuP = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lnbuQ = bitcast i64* %lnbuP to i64*
  %lnbuR = load i64, i64* %lnbuQ, !tbaa !4
  %lnbuS = inttoptr i64 %lnbuR to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbuT = load i64*, i64** %Sp_Var
  %lnbuU = load i64, i64* %R1_Var
  %lnbuV = load i64, i64* %R2_Var
  %lnbuW = load i64, i64* %R3_Var
  %lnbuX = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbuS( i64* %Base_Arg, i64* %lnbuT, i64* %Hp_Arg, i64 %lnbuU, i64 %lnbuV, i64 %lnbuW, i64 %lnbuX, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbtj_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbtj_info$def to i8*)
define internal ghccc void @cbtj_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbtj_info$def to i64)),i64 8), i64 2, i64 47244640288}>
{
cbtj:
  %lsb1J = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbuY = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbtr_info$def to i64
  %lnbuZ = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lnbuY, i64* %lnbuZ, !tbaa !1
  %lnbv0 = load i64, i64* %R1_Var
  store i64 %lnbv0, i64* %lsb1J
  %lnbv1 = getelementptr inbounds i64, i64* %Sp_Arg, i32 1
  %lnbv2 = bitcast i64* %lnbv1 to i64*
  %lnbv3 = load i64, i64* %lnbv2, !tbaa !1
  store i64 %lnbv3, i64* %R1_Var
  %lnbv4 = load i64, i64* %lsb1J
  %lnbv5 = getelementptr inbounds i64, i64* %Sp_Arg, i32 1
  store i64 %lnbv4, i64* %lnbv5, !tbaa !1
  %lnbv6 = bitcast i8* @stg_ap_0_fast to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbv7 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbv6( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbv7, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbtr_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbtr_info$def to i8*)
define internal ghccc void @cbtr_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbtr_info$def to i64)),i64 8), i64 2, i64 47244640288}>
{
cbtr:
  %lsb1K = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbv8 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbtt_info$def to i64
  %lnbv9 = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lnbv8, i64* %lnbv9, !tbaa !1
  %lnbva = load i64, i64* %R1_Var
  store i64 %lnbva, i64* %lsb1K
  %lnbvb = getelementptr inbounds i64, i64* %Sp_Arg, i32 2
  %lnbvc = bitcast i64* %lnbvb to i64*
  %lnbvd = load i64, i64* %lnbvc, !tbaa !1
  store i64 %lnbvd, i64* %R1_Var
  %lnbve = load i64, i64* %lsb1K
  %lnbvf = getelementptr inbounds i64, i64* %Sp_Arg, i32 2
  store i64 %lnbve, i64* %lnbvf, !tbaa !1
  %lnbvg = load i64, i64* %R1_Var
  %lnbvh = and i64 %lnbvg, 7
  %lnbvi = icmp ne i64 %lnbvh, 0
  br i1 %lnbvi, label %ubuf, label %cbtw
cbtw:
  %lnbvk = load i64, i64* %R1_Var
  %lnbvl = inttoptr i64 %lnbvk to i64*
  %lnbvm = load i64, i64* %lnbvl, !tbaa !3
  %lnbvn = inttoptr i64 %lnbvm to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbvo = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbvn( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbvo, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubuf:
  %lnbvp = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbtt_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbvq = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbvp( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbvq, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbtt_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbtt_info$def to i8*)
define internal ghccc void @cbtt_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbtt_info$def to i64)),i64 8), i64 2, i64 47244640288}>
{
cbtt:
  %lsb1J = alloca i64, i32 1
  %lsb1N = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbvr = load i64*, i64** %Sp_Var
  %lnbvs = getelementptr inbounds i64, i64* %lnbvr, i32 1
  %lnbvt = bitcast i64* %lnbvs to i64*
  %lnbvu = load i64, i64* %lnbvt, !tbaa !1
  store i64 %lnbvu, i64* %lsb1J
  %lnbvv = load i64, i64* %R1_Var
  %lnbvw = and i64 %lnbvv, 7
  switch i64 %lnbvw, label %cbtD [i64 1, label %cbtD
i64 2, label %cbu8]
cbtD:
  %lnbvy = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbtA_info$def to i64
  %lnbvx = load i64*, i64** %Sp_Var
  %lnbvz = getelementptr inbounds i64, i64* %lnbvx, i32 -5
  store i64 %lnbvy, i64* %lnbvz, !tbaa !1
  %lnbvC = load i64, i64* %R1_Var
  %lnbvD = add i64 %lnbvC, 7
  %lnbvE = inttoptr i64 %lnbvD to i64*
  %lnbvF = load i64, i64* %lnbvE, !tbaa !3
  store i64 %lnbvF, i64* %lsb1N
  %lnbvG = load i64, i64* %lsb1N
  store i64 %lnbvG, i64* %R3_Var
  %lnbvH = load i64, i64* %lsb1J
  store i64 %lnbvH, i64* %R2_Var
  %lnbvJ = load i64, i64* %lsb1N
  %lnbvI = load i64*, i64** %Sp_Var
  %lnbvK = getelementptr inbounds i64, i64* %lnbvI, i32 -4
  store i64 %lnbvJ, i64* %lnbvK, !tbaa !1
  %lnbvO = load i64, i64* %R1_Var
  %lnbvP = add i64 %lnbvO, 15
  %lnbvQ = inttoptr i64 %lnbvP to i64*
  %lnbvR = load i64, i64* %lnbvQ, !tbaa !3
  %lnbvL = load i64*, i64** %Sp_Var
  %lnbvS = getelementptr inbounds i64, i64* %lnbvL, i32 -3
  store i64 %lnbvR, i64* %lnbvS, !tbaa !1
  %lnbvW = load i64, i64* %R1_Var
  %lnbvX = add i64 %lnbvW, 23
  %lnbvY = inttoptr i64 %lnbvX to i64*
  %lnbvZ = load i64, i64* %lnbvY, !tbaa !3
  %lnbvT = load i64*, i64** %Sp_Var
  %lnbw0 = getelementptr inbounds i64, i64* %lnbvT, i32 -2
  store i64 %lnbvZ, i64* %lnbw0, !tbaa !1
  %lnbw4 = load i64, i64* %R1_Var
  %lnbw5 = add i64 %lnbw4, 31
  %lnbw6 = inttoptr i64 %lnbw5 to i64*
  %lnbw7 = load i64, i64* %lnbw6, !tbaa !3
  %lnbw1 = load i64*, i64** %Sp_Var
  %lnbw8 = getelementptr inbounds i64, i64* %lnbw1, i32 -1
  store i64 %lnbw7, i64* %lnbw8, !tbaa !1
  %lnbwc = load i64, i64* %R1_Var
  %lnbwd = add i64 %lnbwc, 39
  %lnbwe = inttoptr i64 %lnbwd to i64*
  %lnbwf = load i64, i64* %lnbwe, !tbaa !3
  %lnbw9 = load i64*, i64** %Sp_Var
  %lnbwg = getelementptr inbounds i64, i64* %lnbw9, i32 0
  store i64 %lnbwf, i64* %lnbwg, !tbaa !1
  %lnbwh = load i64*, i64** %Sp_Var
  %lnbwi = getelementptr inbounds i64, i64* %lnbwh, i32 -5
  %lnbwj = ptrtoint i64* %lnbwi to i64
  %lnbwk = inttoptr i64 %lnbwj to i64*
  store i64* %lnbwk, i64** %Sp_Var
  %lnbwl = bitcast i8* @integerzmgmp_GHCziIntegerziType_compareInteger_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbwm = load i64*, i64** %Sp_Var
  %lnbwn = load i64*, i64** %Hp_Var
  %lnbwo = load i64, i64* %R1_Var
  %lnbwp = load i64, i64* %R2_Var
  %lnbwq = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbwl( i64* %Base_Arg, i64* %lnbwm, i64* %lnbwn, i64 %lnbwo, i64 %lnbwp, i64 %lnbwq, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbu8:
  %lnbwr = load i64*, i64** %Hp_Var
  %lnbws = getelementptr inbounds i64, i64* %lnbwr, i32 6
  %lnbwt = ptrtoint i64* %lnbws to i64
  %lnbwu = inttoptr i64 %lnbwt to i64*
  store i64* %lnbwu, i64** %Hp_Var
  %lnbwv = load i64*, i64** %Hp_Var
  %lnbww = ptrtoint i64* %lnbwv to i64
  %lnbwx = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnbwy = bitcast i64* %lnbwx to i64*
  %lnbwz = load i64, i64* %lnbwy, !tbaa !4
  %lnbwA = icmp ugt i64 %lnbww, %lnbwz
  br i1 %lnbwA, label %cbub, label %cbua
cbua:
  %lnbwC = ptrtoint i8* @containerszm0zi5zi7zi1_DataziMapziBase_Bin_con_info to i64
  %lnbwB = load i64*, i64** %Hp_Var
  %lnbwD = getelementptr inbounds i64, i64* %lnbwB, i32 -5
  store i64 %lnbwC, i64* %lnbwD, !tbaa !2
  %lnbwF = load i64, i64* %lsb1J
  %lnbwE = load i64*, i64** %Hp_Var
  %lnbwG = getelementptr inbounds i64, i64* %lnbwE, i32 -4
  store i64 %lnbwF, i64* %lnbwG, !tbaa !2
  %lnbwI = load i64*, i64** %Sp_Var
  %lnbwJ = getelementptr inbounds i64, i64* %lnbwI, i32 2
  %lnbwK = bitcast i64* %lnbwJ to i64*
  %lnbwL = load i64, i64* %lnbwK, !tbaa !1
  %lnbwH = load i64*, i64** %Hp_Var
  %lnbwM = getelementptr inbounds i64, i64* %lnbwH, i32 -3
  store i64 %lnbwL, i64* %lnbwM, !tbaa !2
  %lnbwO = ptrtoint i8* @containerszm0zi5zi7zi1_DataziMapziBase_Tip_closure to i64
  %lnbwP = add i64 %lnbwO, 2
  %lnbwN = load i64*, i64** %Hp_Var
  %lnbwQ = getelementptr inbounds i64, i64* %lnbwN, i32 -2
  store i64 %lnbwP, i64* %lnbwQ, !tbaa !2
  %lnbwS = ptrtoint i8* @containerszm0zi5zi7zi1_DataziMapziBase_Tip_closure to i64
  %lnbwT = add i64 %lnbwS, 2
  %lnbwR = load i64*, i64** %Hp_Var
  %lnbwU = getelementptr inbounds i64, i64* %lnbwR, i32 -1
  store i64 %lnbwT, i64* %lnbwU, !tbaa !2
  %lnbwV = load i64*, i64** %Hp_Var
  %lnbwW = getelementptr inbounds i64, i64* %lnbwV, i32 0
  store i64 1, i64* %lnbwW, !tbaa !2
  %lnbwY = load i64*, i64** %Hp_Var
  %lnbwZ = ptrtoint i64* %lnbwY to i64
  %lnbx0 = add i64 %lnbwZ, -39
  store i64 %lnbx0, i64* %R1_Var
  %lnbx1 = load i64*, i64** %Sp_Var
  %lnbx2 = getelementptr inbounds i64, i64* %lnbx1, i32 3
  %lnbx3 = ptrtoint i64* %lnbx2 to i64
  %lnbx4 = inttoptr i64 %lnbx3 to i64*
  store i64* %lnbx4, i64** %Sp_Var
  %lnbx5 = load i64*, i64** %Sp_Var
  %lnbx6 = getelementptr inbounds i64, i64* %lnbx5, i32 0
  %lnbx7 = bitcast i64* %lnbx6 to i64*
  %lnbx8 = load i64, i64* %lnbx7, !tbaa !1
  %lnbx9 = inttoptr i64 %lnbx8 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbxa = load i64*, i64** %Sp_Var
  %lnbxb = load i64*, i64** %Hp_Var
  %lnbxc = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbx9( i64* %Base_Arg, i64* %lnbxa, i64* %lnbxb, i64 %lnbxc, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbub:
  %lnbxd = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 48, i64* %lnbxd, !tbaa !4
  %lnbxe = load i64, i64* %R1_Var
  store i64 %lnbxe, i64* %R1_Var
  %lnbxf = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbxg = load i64*, i64** %Sp_Var
  %lnbxh = load i64*, i64** %Hp_Var
  %lnbxi = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbxf( i64* %Base_Arg, i64* %lnbxg, i64* %lnbxh, i64 %lnbxi, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbtA_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbtA_info$def to i8*)
define internal ghccc void @cbtA_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbtA_info$def to i64)),i64 8), i64 1031, i64 47244640288}>
{
cbtA:
  %lsb1J = alloca i64, i32 1
  %lsb1K = alloca i64, i32 1
  %lsb1P = alloca i64, i32 1
  %lsb1Q = alloca i64, i32 1
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnbxj = load i64*, i64** %Sp_Var
  %lnbxk = getelementptr inbounds i64, i64* %lnbxj, i32 6
  %lnbxl = bitcast i64* %lnbxk to i64*
  %lnbxm = load i64, i64* %lnbxl, !tbaa !1
  store i64 %lnbxm, i64* %lsb1J
  %lnbxn = load i64*, i64** %Sp_Var
  %lnbxo = getelementptr inbounds i64, i64* %lnbxn, i32 7
  %lnbxp = bitcast i64* %lnbxo to i64*
  %lnbxq = load i64, i64* %lnbxp, !tbaa !1
  store i64 %lnbxq, i64* %lsb1K
  %lnbxr = load i64*, i64** %Sp_Var
  %lnbxs = getelementptr inbounds i64, i64* %lnbxr, i32 3
  %lnbxt = bitcast i64* %lnbxs to i64*
  %lnbxu = load i64, i64* %lnbxt, !tbaa !1
  store i64 %lnbxu, i64* %lsb1P
  %lnbxv = load i64*, i64** %Sp_Var
  %lnbxw = getelementptr inbounds i64, i64* %lnbxv, i32 4
  %lnbxx = bitcast i64* %lnbxw to i64*
  %lnbxy = load i64, i64* %lnbxx, !tbaa !1
  store i64 %lnbxy, i64* %lsb1Q
  %lnbxz = load i64, i64* %R1_Var
  %lnbxA = and i64 %lnbxz, 7
  switch i64 %lnbxA, label %cbtK [i64 1, label %cbtK
i64 2, label %cbtS
i64 3, label %cbu0]
cbtK:
  %lnbxC = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbtH_info$def to i64
  %lnbxB = load i64*, i64** %Sp_Var
  %lnbxD = getelementptr inbounds i64, i64* %lnbxB, i32 0
  store i64 %lnbxC, i64* %lnbxD, !tbaa !1
  %lnbxE = load i64, i64* %lsb1P
  store i64 %lnbxE, i64* %R4_Var
  %lnbxF = load i64, i64* %lsb1K
  store i64 %lnbxF, i64* %R3_Var
  %lnbxG = load i64, i64* %lsb1J
  store i64 %lnbxG, i64* %R2_Var
  %lnbxH = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdsinsertzuzdsgo4_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbxI = load i64*, i64** %Sp_Var
  %lnbxJ = load i64*, i64** %Hp_Var
  %lnbxK = load i64, i64* %R1_Var
  %lnbxL = load i64, i64* %R2_Var
  %lnbxM = load i64, i64* %R3_Var
  %lnbxN = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbxH( i64* %Base_Arg, i64* %lnbxI, i64* %lnbxJ, i64 %lnbxK, i64 %lnbxL, i64 %lnbxM, i64 %lnbxN, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbtS:
  %lnbxO = load i64*, i64** %Hp_Var
  %lnbxP = getelementptr inbounds i64, i64* %lnbxO, i32 6
  %lnbxQ = ptrtoint i64* %lnbxP to i64
  %lnbxR = inttoptr i64 %lnbxQ to i64*
  store i64* %lnbxR, i64** %Hp_Var
  %lnbxS = load i64*, i64** %Hp_Var
  %lnbxT = ptrtoint i64* %lnbxS to i64
  %lnbxU = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnbxV = bitcast i64* %lnbxU to i64*
  %lnbxW = load i64, i64* %lnbxV, !tbaa !4
  %lnbxX = icmp ugt i64 %lnbxT, %lnbxW
  br i1 %lnbxX, label %cbtV, label %cbtU
cbtU:
  %lnbxZ = ptrtoint i8* @containerszm0zi5zi7zi1_DataziMapziBase_Bin_con_info to i64
  %lnbxY = load i64*, i64** %Hp_Var
  %lnby0 = getelementptr inbounds i64, i64* %lnbxY, i32 -5
  store i64 %lnbxZ, i64* %lnby0, !tbaa !2
  %lnby2 = load i64, i64* %lsb1J
  %lnby1 = load i64*, i64** %Hp_Var
  %lnby3 = getelementptr inbounds i64, i64* %lnby1, i32 -4
  store i64 %lnby2, i64* %lnby3, !tbaa !2
  %lnby5 = load i64, i64* %lsb1K
  %lnby4 = load i64*, i64** %Hp_Var
  %lnby6 = getelementptr inbounds i64, i64* %lnby4, i32 -3
  store i64 %lnby5, i64* %lnby6, !tbaa !2
  %lnby8 = load i64, i64* %lsb1P
  %lnby7 = load i64*, i64** %Hp_Var
  %lnby9 = getelementptr inbounds i64, i64* %lnby7, i32 -2
  store i64 %lnby8, i64* %lnby9, !tbaa !2
  %lnbyb = load i64, i64* %lsb1Q
  %lnbya = load i64*, i64** %Hp_Var
  %lnbyc = getelementptr inbounds i64, i64* %lnbya, i32 -1
  store i64 %lnbyb, i64* %lnbyc, !tbaa !2
  %lnbye = load i64*, i64** %Sp_Var
  %lnbyf = getelementptr inbounds i64, i64* %lnbye, i32 5
  %lnbyg = bitcast i64* %lnbyf to i64*
  %lnbyh = load i64, i64* %lnbyg, !tbaa !1
  %lnbyd = load i64*, i64** %Hp_Var
  %lnbyi = getelementptr inbounds i64, i64* %lnbyd, i32 0
  store i64 %lnbyh, i64* %lnbyi, !tbaa !2
  %lnbyk = load i64*, i64** %Hp_Var
  %lnbyl = ptrtoint i64* %lnbyk to i64
  %lnbym = add i64 %lnbyl, -39
  store i64 %lnbym, i64* %R1_Var
  %lnbyn = load i64*, i64** %Sp_Var
  %lnbyo = getelementptr inbounds i64, i64* %lnbyn, i32 8
  %lnbyp = ptrtoint i64* %lnbyo to i64
  %lnbyq = inttoptr i64 %lnbyp to i64*
  store i64* %lnbyq, i64** %Sp_Var
  %lnbyr = load i64*, i64** %Sp_Var
  %lnbys = getelementptr inbounds i64, i64* %lnbyr, i32 0
  %lnbyt = bitcast i64* %lnbys to i64*
  %lnbyu = load i64, i64* %lnbyt, !tbaa !1
  %lnbyv = inttoptr i64 %lnbyu to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbyw = load i64*, i64** %Sp_Var
  %lnbyx = load i64*, i64** %Hp_Var
  %lnbyy = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbyv( i64* %Base_Arg, i64* %lnbyw, i64* %lnbyx, i64 %lnbyy, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbtV:
  %lnbyz = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 48, i64* %lnbyz, !tbaa !4
  %lnbyA = load i64, i64* %R1_Var
  store i64 %lnbyA, i64* %R1_Var
  %lnbyB = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbyC = load i64*, i64** %Sp_Var
  %lnbyD = load i64*, i64** %Hp_Var
  %lnbyE = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbyB( i64* %Base_Arg, i64* %lnbyC, i64* %lnbyD, i64 %lnbyE, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbu0:
  %lnbyG = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbtY_info$def to i64
  %lnbyF = load i64*, i64** %Sp_Var
  %lnbyH = getelementptr inbounds i64, i64* %lnbyF, i32 0
  store i64 %lnbyG, i64* %lnbyH, !tbaa !1
  %lnbyI = load i64, i64* %lsb1Q
  store i64 %lnbyI, i64* %R4_Var
  %lnbyJ = load i64, i64* %lsb1K
  store i64 %lnbyJ, i64* %R3_Var
  %lnbyK = load i64, i64* %lsb1J
  store i64 %lnbyK, i64* %R2_Var
  %lnbyL = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdsinsertzuzdsgo4_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbyM = load i64*, i64** %Sp_Var
  %lnbyN = load i64*, i64** %Hp_Var
  %lnbyO = load i64, i64* %R1_Var
  %lnbyP = load i64, i64* %R2_Var
  %lnbyQ = load i64, i64* %R3_Var
  %lnbyR = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbyL( i64* %Base_Arg, i64* %lnbyM, i64* %lnbyN, i64 %lnbyO, i64 %lnbyP, i64 %lnbyQ, i64 %lnbyR, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbtY_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbtY_info$def to i8*)
define internal ghccc void @cbtY_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbtY_info$def to i64)),i64 8), i64 7687, i64 4294967328}>
{
cbtY:
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  store i64 %R1_Arg, i64* %R5_Var
  %lnbyS = load i64*, i64** %Sp_Var
  %lnbyT = getelementptr inbounds i64, i64* %lnbyS, i32 3
  %lnbyU = bitcast i64* %lnbyT to i64*
  %lnbyV = load i64, i64* %lnbyU, !tbaa !1
  store i64 %lnbyV, i64* %R4_Var
  %lnbyW = load i64*, i64** %Sp_Var
  %lnbyX = getelementptr inbounds i64, i64* %lnbyW, i32 2
  %lnbyY = bitcast i64* %lnbyX to i64*
  %lnbyZ = load i64, i64* %lnbyY, !tbaa !1
  store i64 %lnbyZ, i64* %R3_Var
  %lnbz0 = load i64*, i64** %Sp_Var
  %lnbz1 = getelementptr inbounds i64, i64* %lnbz0, i32 1
  %lnbz2 = bitcast i64* %lnbz1 to i64*
  %lnbz3 = load i64, i64* %lnbz2, !tbaa !1
  store i64 %lnbz3, i64* %R2_Var
  %lnbz4 = load i64*, i64** %Sp_Var
  %lnbz5 = getelementptr inbounds i64, i64* %lnbz4, i32 8
  %lnbz6 = ptrtoint i64* %lnbz5 to i64
  %lnbz7 = inttoptr i64 %lnbz6 to i64*
  store i64* %lnbz7, i64** %Sp_Var
  %lnbz8 = bitcast i8* @containerszm0zi5zi7zi1_DataziMapziBase_balanceR_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbz9 = load i64*, i64** %Sp_Var
  %lnbza = load i64, i64* %R2_Var
  %lnbzb = load i64, i64* %R3_Var
  %lnbzc = load i64, i64* %R4_Var
  %lnbzd = load i64, i64* %R5_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbz8( i64* %Base_Arg, i64* %lnbz9, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnbza, i64 %lnbzb, i64 %lnbzc, i64 %lnbzd, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbtH_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbtH_info$def to i8*)
define internal ghccc void @cbtH_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbtH_info$def to i64)),i64 16), i64 7431, i64 4294967328}>
{
cbtH:
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnbze = load i64*, i64** %Sp_Var
  %lnbzf = getelementptr inbounds i64, i64* %lnbze, i32 4
  %lnbzg = bitcast i64* %lnbzf to i64*
  %lnbzh = load i64, i64* %lnbzg, !tbaa !1
  store i64 %lnbzh, i64* %R5_Var
  store i64 %R1_Arg, i64* %R4_Var
  %lnbzi = load i64*, i64** %Sp_Var
  %lnbzj = getelementptr inbounds i64, i64* %lnbzi, i32 2
  %lnbzk = bitcast i64* %lnbzj to i64*
  %lnbzl = load i64, i64* %lnbzk, !tbaa !1
  store i64 %lnbzl, i64* %R3_Var
  %lnbzm = load i64*, i64** %Sp_Var
  %lnbzn = getelementptr inbounds i64, i64* %lnbzm, i32 1
  %lnbzo = bitcast i64* %lnbzn to i64*
  %lnbzp = load i64, i64* %lnbzo, !tbaa !1
  store i64 %lnbzp, i64* %R2_Var
  %lnbzq = load i64*, i64** %Sp_Var
  %lnbzr = getelementptr inbounds i64, i64* %lnbzq, i32 8
  %lnbzs = ptrtoint i64* %lnbzr to i64
  %lnbzt = inttoptr i64 %lnbzs to i64*
  store i64* %lnbzt, i64** %Sp_Var
  %lnbzu = bitcast i8* @containerszm0zi5zi7zi1_DataziMapziBase_balanceL_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbzv = load i64*, i64** %Sp_Var
  %lnbzw = load i64, i64* %R2_Var
  %lnbzx = load i64, i64* %R3_Var
  %lnbzy = load i64, i64* %R4_Var
  %lnbzz = load i64, i64* %R5_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbzu( i64* %Base_Arg, i64* %lnbzv, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnbzw, i64 %lnbzx, i64 %lnbzy, i64 %lnbzz, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_verbose_closure_struct = type <{i64}>
@Disk_verbose_closure$def = internal global %Disk_verbose_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_False_static_info to i64)}>
@Disk_verbose_closure = alias i8* bitcast (%Disk_verbose_closure_struct* @Disk_verbose_closure$def to i8*)
%Disk_reallySync_closure_struct = type <{i64}>
@Disk_reallySync_closure$def = internal global %Disk_reallySync_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_True_static_info to i64)}>
@Disk_reallySync_closure = alias i8* bitcast (%Disk_reallySync_closure_struct* @Disk_reallySync_closure$def to i8*)
%Disk_debugSyncs_closure_struct = type <{i64}>
@Disk_debugSyncs_closure$def = internal global %Disk_debugSyncs_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_False_static_info to i64)}>
@Disk_debugSyncs_closure = alias i8* bitcast (%Disk_debugSyncs_closure_struct* @Disk_debugSyncs_closure$def to i8*)
%Disk_clearzustats2_closure_struct = type <{i64, i64, i64, i64}>
@Disk_clearzustats2_closure$def = internal global %Disk_clearzustats2_closure_struct<{i64 ptrtoint (i8* @Disk_Stats_static_info to i64), i64 0, i64 0, i64 0}>
@Disk_clearzustats2_closure = alias i8* bitcast (%Disk_clearzustats2_closure_struct* @Disk_clearzustats2_closure$def to i8*)
%Disk_clearzustats1_closure_struct = type <{i64}>
@Disk_clearzustats1_closure$def = internal global %Disk_clearzustats1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_clearzustats1_info$def to i64)}>
@Disk_clearzustats1_closure = alias i8* bitcast (%Disk_clearzustats1_closure_struct* @Disk_clearzustats1_closure$def to i8*)
@Disk_clearzustats1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_clearzustats1_info$def to i8*)
define ghccc void @Disk_clearzustats1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934597, i64 0, i64 15}>
{
cbzS:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnbA2 = load i64*, i64** %Sp_Var
  %lnbA3 = getelementptr inbounds i64, i64* %lnbA2, i32 -1
  %lnbA4 = ptrtoint i64* %lnbA3 to i64
  %lnbA5 = icmp ult i64 %lnbA4, %SpLim_Arg
  br i1 %lnbA5, label %cbzT, label %cbzU
cbzU:
  %lnbA7 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbzP_info$def to i64
  %lnbA6 = load i64*, i64** %Sp_Var
  %lnbA8 = getelementptr inbounds i64, i64* %lnbA6, i32 -1
  store i64 %lnbA7, i64* %lnbA8, !tbaa !1
  %lnbA9 = load i64, i64* %R2_Var
  store i64 %lnbA9, i64* %R1_Var
  %lnbAa = load i64*, i64** %Sp_Var
  %lnbAb = getelementptr inbounds i64, i64* %lnbAa, i32 -1
  %lnbAc = ptrtoint i64* %lnbAb to i64
  %lnbAd = inttoptr i64 %lnbAc to i64*
  store i64* %lnbAd, i64** %Sp_Var
  %lnbAe = load i64, i64* %R1_Var
  %lnbAf = and i64 %lnbAe, 7
  %lnbAg = icmp ne i64 %lnbAf, 0
  br i1 %lnbAg, label %ubA1, label %cbzQ
cbzQ:
  %lnbAi = load i64, i64* %R1_Var
  %lnbAj = inttoptr i64 %lnbAi to i64*
  %lnbAk = load i64, i64* %lnbAj, !tbaa !3
  %lnbAl = inttoptr i64 %lnbAk to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbAm = load i64*, i64** %Sp_Var
  %lnbAn = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbAl( i64* %Base_Arg, i64* %lnbAm, i64* %Hp_Arg, i64 %lnbAn, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubA1:
  %lnbAo = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbzP_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbAp = load i64*, i64** %Sp_Var
  %lnbAq = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbAo( i64* %Base_Arg, i64* %lnbAp, i64* %Hp_Arg, i64 %lnbAq, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbzT:
  %lnbAr = load i64, i64* %R2_Var
  store i64 %lnbAr, i64* %R2_Var
  %lnbAs = ptrtoint %Disk_clearzustats1_closure_struct* @Disk_clearzustats1_closure$def to i64
  store i64 %lnbAs, i64* %R1_Var
  %lnbAt = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lnbAu = bitcast i64* %lnbAt to i64*
  %lnbAv = load i64, i64* %lnbAu, !tbaa !4
  %lnbAw = inttoptr i64 %lnbAv to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbAx = load i64*, i64** %Sp_Var
  %lnbAy = load i64, i64* %R1_Var
  %lnbAz = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbAw( i64* %Base_Arg, i64* %lnbAx, i64* %Hp_Arg, i64 %lnbAy, i64 %lnbAz, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbzP_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbzP_info$def to i8*)
define internal ghccc void @cbzP_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
cbzP:
  %lsb1Y = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnbAC = load i64, i64* %R1_Var
  %lnbAD = add i64 %lnbAC, 7
  %lnbAE = inttoptr i64 %lnbAD to i64*
  %lnbAF = load i64, i64* %lnbAE, !tbaa !3
  store i64 %lnbAF, i64* %lsb1Y
  %lnbAG = load i64, i64* %lsb1Y
  %lnbAH = add i64 %lnbAG, 8
  %lnbAI = ptrtoint %Disk_clearzustats2_closure_struct* @Disk_clearzustats2_closure$def to i64
  %lnbAJ = add i64 %lnbAI, 1
  %lnbAK = inttoptr i64 %lnbAH to i64*
  store i64 %lnbAJ, i64* %lnbAK, !tbaa !5
  %lnbAL = ptrtoint i64* %Base_Arg to i64
  %lnbAM = inttoptr i64 %lnbAL to i8*
  %lnbAN = load i64, i64* %lsb1Y
  %lnbAO = inttoptr i64 %lnbAN to i8*
  %lnbAP = bitcast i8* @dirty_MUT_VAR to void (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  call ccc void (i8*, i8*) %lnbAP( i8* %lnbAM, i8* %lnbAO ) nounwind
  %lnbAQ = ptrtoint i8* @ghczmprim_GHCziTuple_Z0T_closure to i64
  %lnbAR = add i64 %lnbAQ, 1
  store i64 %lnbAR, i64* %R1_Var
  %lnbAS = load i64*, i64** %Sp_Var
  %lnbAT = getelementptr inbounds i64, i64* %lnbAS, i32 1
  %lnbAU = ptrtoint i64* %lnbAT to i64
  %lnbAV = inttoptr i64 %lnbAU to i64*
  store i64* %lnbAV, i64** %Sp_Var
  %lnbAW = load i64*, i64** %Sp_Var
  %lnbAX = getelementptr inbounds i64, i64* %lnbAW, i32 0
  %lnbAY = bitcast i64* %lnbAX to i64*
  %lnbAZ = load i64, i64* %lnbAY, !tbaa !1
  %lnbB0 = inttoptr i64 %lnbAZ to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbB1 = load i64*, i64** %Sp_Var
  %lnbB2 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbB0( i64* %Base_Arg, i64* %lnbB1, i64* %Hp_Arg, i64 %lnbB2, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_clearzustats_closure_struct = type <{i64}>
@Disk_clearzustats_closure$def = internal global %Disk_clearzustats_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_clearzustats_info$def to i64)}>
@Disk_clearzustats_closure = alias i8* bitcast (%Disk_clearzustats_closure_struct* @Disk_clearzustats_closure$def to i8*)
@Disk_clearzustats_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_clearzustats_info$def to i8*)
define ghccc void @Disk_clearzustats_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934597, i64 0, i64 15}>
{
cbB8:
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnbBb = load i64, i64* %R2_Var
  store i64 %lnbBb, i64* %R2_Var
  %lnbBc = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_clearzustats1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbBd = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbBc( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnbBd, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%cbBf_str_struct = type <{[5 x i8]}>
%Disk_zdtrModule2_closure_struct = type <{i64, i64}>
@cbBf_str$def = internal constant %cbBf_str_struct<{[5 x i8] [i8 109, i8 97, i8 105, i8 110, i8 0]}>
@cbBf_str = internal alias i8* bitcast (%cbBf_str_struct* @cbBf_str$def to i8*)
@Disk_zdtrModule2_closure$def = internal global %Disk_zdtrModule2_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TrNameS_static_info to i64), i64 ptrtoint (i8* @cbBf_str to i64)}>
@Disk_zdtrModule2_closure = alias i8* bitcast (%Disk_zdtrModule2_closure_struct* @Disk_zdtrModule2_closure$def to i8*)
%cbBh_str_struct = type <{[5 x i8]}>
%Disk_zdtrModule1_closure_struct = type <{i64, i64}>
@cbBh_str$def = internal constant %cbBh_str_struct<{[5 x i8] [i8 68, i8 105, i8 115, i8 107, i8 0]}>
@cbBh_str = internal alias i8* bitcast (%cbBh_str_struct* @cbBh_str$def to i8*)
@Disk_zdtrModule1_closure$def = internal global %Disk_zdtrModule1_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TrNameS_static_info to i64), i64 ptrtoint (i8* @cbBh_str to i64)}>
@Disk_zdtrModule1_closure = alias i8* bitcast (%Disk_zdtrModule1_closure_struct* @Disk_zdtrModule1_closure$def to i8*)
%Disk_zdtrModule_closure_struct = type <{i64, i64, i64, i64}>
@Disk_zdtrModule_closure$def = internal global %Disk_zdtrModule_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_Module_static_info to i64), i64 add (i64 ptrtoint (%Disk_zdtrModule2_closure_struct* @Disk_zdtrModule2_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%Disk_zdtrModule1_closure_struct* @Disk_zdtrModule1_closure$def to i64),i64 1), i64 3}>
@Disk_zdtrModule_closure = alias i8* bitcast (%Disk_zdtrModule_closure_struct* @Disk_zdtrModule_closure$def to i8*)
%cbBk_str_struct = type <{[7 x i8]}>
%Disk_zdtczqStats1_closure_struct = type <{i64, i64}>
@cbBk_str$def = internal constant %cbBk_str_struct<{[7 x i8] [i8 39, i8 83, i8 116, i8 97, i8 116, i8 115, i8 0]}>
@cbBk_str = internal alias i8* bitcast (%cbBk_str_struct* @cbBk_str$def to i8*)
@Disk_zdtczqStats1_closure$def = internal global %Disk_zdtczqStats1_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TrNameS_static_info to i64), i64 ptrtoint (i8* @cbBk_str to i64)}>
@Disk_zdtczqStats1_closure = alias i8* bitcast (%Disk_zdtczqStats1_closure_struct* @Disk_zdtczqStats1_closure$def to i8*)
%Disk_zdtczqStats_closure_struct = type <{i64, i64, i64, i64, i64, i64}>
@Disk_zdtczqStats_closure$def = internal global %Disk_zdtczqStats_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TyCon_static_info to i64), i64 add (i64 ptrtoint (%Disk_zdtrModule_closure_struct* @Disk_zdtrModule_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%Disk_zdtczqStats1_closure_struct* @Disk_zdtczqStats1_closure$def to i64),i64 1), i64 -4764756283864861065, i64 6569553883726356705, i64 3}>
@Disk_zdtczqStats_closure = alias i8* bitcast (%Disk_zdtczqStats_closure_struct* @Disk_zdtczqStats_closure$def to i8*)
%cbBn_str_struct = type <{[10 x i8]}>
%Disk_zdtcDiskStats1_closure_struct = type <{i64, i64}>
@cbBn_str$def = internal constant %cbBn_str_struct<{[10 x i8] [i8 68, i8 105, i8 115, i8 107, i8 83, i8 116, i8 97, i8 116, i8 115, i8 0]}>
@cbBn_str = internal alias i8* bitcast (%cbBn_str_struct* @cbBn_str$def to i8*)
@Disk_zdtcDiskStats1_closure$def = internal global %Disk_zdtcDiskStats1_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TrNameS_static_info to i64), i64 ptrtoint (i8* @cbBn_str to i64)}>
@Disk_zdtcDiskStats1_closure = alias i8* bitcast (%Disk_zdtcDiskStats1_closure_struct* @Disk_zdtcDiskStats1_closure$def to i8*)
%Disk_zdtcDiskStats_closure_struct = type <{i64, i64, i64, i64, i64, i64}>
@Disk_zdtcDiskStats_closure$def = internal global %Disk_zdtcDiskStats_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TyCon_static_info to i64), i64 add (i64 ptrtoint (%Disk_zdtrModule_closure_struct* @Disk_zdtrModule_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%Disk_zdtcDiskStats1_closure_struct* @Disk_zdtcDiskStats1_closure$def to i64),i64 1), i64 1935875407529589416, i64 2312477101651139582, i64 3}>
@Disk_zdtcDiskStats_closure = alias i8* bitcast (%Disk_zdtcDiskStats_closure_struct* @Disk_zdtcDiskStats_closure$def to i8*)
%cbBq_str_struct = type <{[4 x i8]}>
%Disk_zdtczqFL1_closure_struct = type <{i64, i64}>
@cbBq_str$def = internal constant %cbBq_str_struct<{[4 x i8] [i8 39, i8 70, i8 76, i8 0]}>
@cbBq_str = internal alias i8* bitcast (%cbBq_str_struct* @cbBq_str$def to i8*)
@Disk_zdtczqFL1_closure$def = internal global %Disk_zdtczqFL1_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TrNameS_static_info to i64), i64 ptrtoint (i8* @cbBq_str to i64)}>
@Disk_zdtczqFL1_closure = alias i8* bitcast (%Disk_zdtczqFL1_closure_struct* @Disk_zdtczqFL1_closure$def to i8*)
%Disk_zdtczqFL_closure_struct = type <{i64, i64, i64, i64, i64, i64}>
@Disk_zdtczqFL_closure$def = internal global %Disk_zdtczqFL_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TyCon_static_info to i64), i64 add (i64 ptrtoint (%Disk_zdtrModule_closure_struct* @Disk_zdtrModule_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%Disk_zdtczqFL1_closure_struct* @Disk_zdtczqFL1_closure$def to i64),i64 1), i64 -255461596061048519, i64 2906434361066789569, i64 3}>
@Disk_zdtczqFL_closure = alias i8* bitcast (%Disk_zdtczqFL_closure_struct* @Disk_zdtczqFL_closure$def to i8*)
%cbBt_str_struct = type <{[9 x i8]}>
%Disk_zdtcFlushLog1_closure_struct = type <{i64, i64}>
@cbBt_str$def = internal constant %cbBt_str_struct<{[9 x i8] [i8 70, i8 108, i8 117, i8 115, i8 104, i8 76, i8 111, i8 103, i8 0]}>
@cbBt_str = internal alias i8* bitcast (%cbBt_str_struct* @cbBt_str$def to i8*)
@Disk_zdtcFlushLog1_closure$def = internal global %Disk_zdtcFlushLog1_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TrNameS_static_info to i64), i64 ptrtoint (i8* @cbBt_str to i64)}>
@Disk_zdtcFlushLog1_closure = alias i8* bitcast (%Disk_zdtcFlushLog1_closure_struct* @Disk_zdtcFlushLog1_closure$def to i8*)
%Disk_zdtcFlushLog_closure_struct = type <{i64, i64, i64, i64, i64, i64}>
@Disk_zdtcFlushLog_closure$def = internal global %Disk_zdtcFlushLog_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TyCon_static_info to i64), i64 add (i64 ptrtoint (%Disk_zdtrModule_closure_struct* @Disk_zdtrModule_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%Disk_zdtcFlushLog1_closure_struct* @Disk_zdtcFlushLog1_closure$def to i64),i64 1), i64 -5949603735508298445, i64 -1070193729100987922, i64 3}>
@Disk_zdtcFlushLog_closure = alias i8* bitcast (%Disk_zdtcFlushLog_closure_struct* @Disk_zdtcFlushLog_closure$def to i8*)
%cbBw_str_struct = type <{[4 x i8]}>
%Disk_zdtczqVS1_closure_struct = type <{i64, i64}>
@cbBw_str$def = internal constant %cbBw_str_struct<{[4 x i8] [i8 39, i8 86, i8 83, i8 0]}>
@cbBw_str = internal alias i8* bitcast (%cbBw_str_struct* @cbBw_str$def to i8*)
@Disk_zdtczqVS1_closure$def = internal global %Disk_zdtczqVS1_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TrNameS_static_info to i64), i64 ptrtoint (i8* @cbBw_str to i64)}>
@Disk_zdtczqVS1_closure = alias i8* bitcast (%Disk_zdtczqVS1_closure_struct* @Disk_zdtczqVS1_closure$def to i8*)
%Disk_zdtczqVS_closure_struct = type <{i64, i64, i64, i64, i64, i64}>
@Disk_zdtczqVS_closure$def = internal global %Disk_zdtczqVS_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TyCon_static_info to i64), i64 add (i64 ptrtoint (%Disk_zdtrModule_closure_struct* @Disk_zdtrModule_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%Disk_zdtczqVS1_closure_struct* @Disk_zdtczqVS1_closure$def to i64),i64 1), i64 1526219101404828458, i64 -2331539717400308248, i64 3}>
@Disk_zdtczqVS_closure = alias i8* bitcast (%Disk_zdtczqVS_closure_struct* @Disk_zdtczqVS_closure$def to i8*)
%cbBz_str_struct = type <{[9 x i8]}>
%Disk_zdtcVarStore1_closure_struct = type <{i64, i64}>
@cbBz_str$def = internal constant %cbBz_str_struct<{[9 x i8] [i8 86, i8 97, i8 114, i8 83, i8 116, i8 111, i8 114, i8 101, i8 0]}>
@cbBz_str = internal alias i8* bitcast (%cbBz_str_struct* @cbBz_str$def to i8*)
@Disk_zdtcVarStore1_closure$def = internal global %Disk_zdtcVarStore1_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TrNameS_static_info to i64), i64 ptrtoint (i8* @cbBz_str to i64)}>
@Disk_zdtcVarStore1_closure = alias i8* bitcast (%Disk_zdtcVarStore1_closure_struct* @Disk_zdtcVarStore1_closure$def to i8*)
%Disk_zdtcVarStore_closure_struct = type <{i64, i64, i64, i64, i64, i64}>
@Disk_zdtcVarStore_closure$def = internal global %Disk_zdtcVarStore_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TyCon_static_info to i64), i64 add (i64 ptrtoint (%Disk_zdtrModule_closure_struct* @Disk_zdtrModule_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%Disk_zdtcVarStore1_closure_struct* @Disk_zdtcVarStore1_closure$def to i64),i64 1), i64 4433262000258959044, i64 -3781993493689854512, i64 3}>
@Disk_zdtcVarStore_closure = alias i8* bitcast (%Disk_zdtcVarStore_closure_struct* @Disk_zdtcVarStore_closure$def to i8*)
%cbBC_str_struct = type <{[3 x i8]}>
%Disk_zdtczqS1_closure_struct = type <{i64, i64}>
@cbBC_str$def = internal constant %cbBC_str_struct<{[3 x i8] [i8 39, i8 83, i8 0]}>
@cbBC_str = internal alias i8* bitcast (%cbBC_str_struct* @cbBC_str$def to i8*)
@Disk_zdtczqS1_closure$def = internal global %Disk_zdtczqS1_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TrNameS_static_info to i64), i64 ptrtoint (i8* @cbBC_str to i64)}>
@Disk_zdtczqS1_closure = alias i8* bitcast (%Disk_zdtczqS1_closure_struct* @Disk_zdtczqS1_closure$def to i8*)
%Disk_zdtczqS_closure_struct = type <{i64, i64, i64, i64, i64, i64}>
@Disk_zdtczqS_closure$def = internal global %Disk_zdtczqS_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TyCon_static_info to i64), i64 add (i64 ptrtoint (%Disk_zdtrModule_closure_struct* @Disk_zdtrModule_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%Disk_zdtczqS1_closure_struct* @Disk_zdtczqS1_closure$def to i64),i64 1), i64 -8378446406242839334, i64 5085959682913743084, i64 3}>
@Disk_zdtczqS_closure = alias i8* bitcast (%Disk_zdtczqS_closure_struct* @Disk_zdtczqS_closure$def to i8*)
%cbBF_str_struct = type <{[10 x i8]}>
%Disk_zdtcDiskState1_closure_struct = type <{i64, i64}>
@cbBF_str$def = internal constant %cbBF_str_struct<{[10 x i8] [i8 68, i8 105, i8 115, i8 107, i8 83, i8 116, i8 97, i8 116, i8 101, i8 0]}>
@cbBF_str = internal alias i8* bitcast (%cbBF_str_struct* @cbBF_str$def to i8*)
@Disk_zdtcDiskState1_closure$def = internal global %Disk_zdtcDiskState1_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TrNameS_static_info to i64), i64 ptrtoint (i8* @cbBF_str to i64)}>
@Disk_zdtcDiskState1_closure = alias i8* bitcast (%Disk_zdtcDiskState1_closure_struct* @Disk_zdtcDiskState1_closure$def to i8*)
%Disk_zdtcDiskState_closure_struct = type <{i64, i64, i64, i64, i64, i64}>
@Disk_zdtcDiskState_closure$def = internal global %Disk_zdtcDiskState_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TyCon_static_info to i64), i64 add (i64 ptrtoint (%Disk_zdtrModule_closure_struct* @Disk_zdtrModule_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%Disk_zdtcDiskState1_closure_struct* @Disk_zdtcDiskState1_closure$def to i64),i64 1), i64 -3445878457645589459, i64 -8650281819059512007, i64 3}>
@Disk_zdtcDiskState_closure = alias i8* bitcast (%Disk_zdtcDiskState_closure_struct* @Disk_zdtcDiskState_closure$def to i8*)
%rb0s_closure_struct = type <{i64, i64, i64, i64}>
%cbBO_str_struct = type <{[5 x i8]}>
@rb0s_closure$def = internal global %rb0s_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rb0s_info$def to i64), i64 0, i64 0, i64 0}>
@rb0s_closure = internal alias i8* bitcast (%rb0s_closure_struct* @rb0s_closure$def to i8*)
@cbBO_str$def = internal constant %cbBO_str_struct<{[5 x i8] [i8 109, i8 97, i8 105, i8 110, i8 0]}>
@cbBO_str = internal alias i8* bitcast (%cbBO_str_struct* @cbBO_str$def to i8*)
@rb0s_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rb0s_info$def to i8*)
define internal ghccc void @rb0s_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 22}>
{
cbBP:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %lcbBL = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbBS = load i64*, i64** %Sp_Var
  %lnbBT = getelementptr inbounds i64, i64* %lnbBS, i32 -2
  %lnbBU = ptrtoint i64* %lnbBT to i64
  %lnbBV = icmp ult i64 %lnbBU, %SpLim_Arg
  br i1 %lnbBV, label %cbBQ, label %cbBR
cbBR:
  %lnbBW = ptrtoint i64* %Base_Arg to i64
  %lnbBX = inttoptr i64 %lnbBW to i8*
  %lnbBY = load i64, i64* %R1_Var
  %lnbBZ = inttoptr i64 %lnbBY to i8*
  %lnbC0 = bitcast i8* @newCAF to i8* (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lnbC1 = call ccc i8* (i8*, i8*) %lnbC0( i8* %lnbBX, i8* %lnbBZ ) nounwind
  %lnbC2 = ptrtoint i8* %lnbC1 to i64
  store i64 %lnbC2, i64* %lcbBL
  %lnbC3 = load i64, i64* %lcbBL
  %lnbC4 = icmp eq i64 %lnbC3, 0
  br i1 %lnbC4, label %cbBN, label %cbBM
cbBM:
  %lnbC6 = ptrtoint i8* @stg_bh_upd_frame_info to i64
  %lnbC5 = load i64*, i64** %Sp_Var
  %lnbC7 = getelementptr inbounds i64, i64* %lnbC5, i32 -2
  store i64 %lnbC6, i64* %lnbC7, !tbaa !1
  %lnbC9 = load i64, i64* %lcbBL
  %lnbC8 = load i64*, i64** %Sp_Var
  %lnbCa = getelementptr inbounds i64, i64* %lnbC8, i32 -1
  store i64 %lnbC9, i64* %lnbCa, !tbaa !1
  %lnbCb = ptrtoint %cbBO_str_struct* @cbBO_str$def to i64
  store i64 %lnbCb, i64* %R2_Var
  %lnbCc = load i64*, i64** %Sp_Var
  %lnbCd = getelementptr inbounds i64, i64* %lnbCc, i32 -2
  %lnbCe = ptrtoint i64* %lnbCd to i64
  %lnbCf = inttoptr i64 %lnbCe to i64*
  store i64* %lnbCf, i64** %Sp_Var
  %lnbCg = bitcast i8* @ghczmprim_GHCziCString_unpackCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbCh = load i64*, i64** %Sp_Var
  %lnbCi = load i64, i64* %R1_Var
  %lnbCj = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbCg( i64* %Base_Arg, i64* %lnbCh, i64* %Hp_Arg, i64 %lnbCi, i64 %lnbCj, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbBN:
  %lnbCl = load i64, i64* %R1_Var
  %lnbCm = inttoptr i64 %lnbCl to i64*
  %lnbCn = load i64, i64* %lnbCm, !tbaa !3
  %lnbCo = inttoptr i64 %lnbCn to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbCp = load i64*, i64** %Sp_Var
  %lnbCq = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbCo( i64* %Base_Arg, i64* %lnbCp, i64* %Hp_Arg, i64 %lnbCq, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbBQ:
  %lnbCr = load i64, i64* %R1_Var
  store i64 %lnbCr, i64* %R1_Var
  %lnbCs = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lnbCt = bitcast i64* %lnbCs to i64*
  %lnbCu = load i64, i64* %lnbCt, !tbaa !4
  %lnbCv = inttoptr i64 %lnbCu to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbCw = load i64*, i64** %Sp_Var
  %lnbCx = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbCv( i64* %Base_Arg, i64* %lnbCw, i64* %Hp_Arg, i64 %lnbCx, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%rb0t_closure_struct = type <{i64, i64, i64, i64}>
%cbCF_str_struct = type <{[5 x i8]}>
@rb0t_closure$def = internal global %rb0t_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rb0t_info$def to i64), i64 0, i64 0, i64 0}>
@rb0t_closure = internal alias i8* bitcast (%rb0t_closure_struct* @rb0t_closure$def to i8*)
@cbCF_str$def = internal constant %cbCF_str_struct<{[5 x i8] [i8 68, i8 105, i8 115, i8 107, i8 0]}>
@cbCF_str = internal alias i8* bitcast (%cbCF_str_struct* @cbCF_str$def to i8*)
@rb0t_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rb0t_info$def to i8*)
define internal ghccc void @rb0t_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 22}>
{
cbCG:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %lcbCC = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbCJ = load i64*, i64** %Sp_Var
  %lnbCK = getelementptr inbounds i64, i64* %lnbCJ, i32 -2
  %lnbCL = ptrtoint i64* %lnbCK to i64
  %lnbCM = icmp ult i64 %lnbCL, %SpLim_Arg
  br i1 %lnbCM, label %cbCH, label %cbCI
cbCI:
  %lnbCN = ptrtoint i64* %Base_Arg to i64
  %lnbCO = inttoptr i64 %lnbCN to i8*
  %lnbCP = load i64, i64* %R1_Var
  %lnbCQ = inttoptr i64 %lnbCP to i8*
  %lnbCR = bitcast i8* @newCAF to i8* (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lnbCS = call ccc i8* (i8*, i8*) %lnbCR( i8* %lnbCO, i8* %lnbCQ ) nounwind
  %lnbCT = ptrtoint i8* %lnbCS to i64
  store i64 %lnbCT, i64* %lcbCC
  %lnbCU = load i64, i64* %lcbCC
  %lnbCV = icmp eq i64 %lnbCU, 0
  br i1 %lnbCV, label %cbCE, label %cbCD
cbCD:
  %lnbCX = ptrtoint i8* @stg_bh_upd_frame_info to i64
  %lnbCW = load i64*, i64** %Sp_Var
  %lnbCY = getelementptr inbounds i64, i64* %lnbCW, i32 -2
  store i64 %lnbCX, i64* %lnbCY, !tbaa !1
  %lnbD0 = load i64, i64* %lcbCC
  %lnbCZ = load i64*, i64** %Sp_Var
  %lnbD1 = getelementptr inbounds i64, i64* %lnbCZ, i32 -1
  store i64 %lnbD0, i64* %lnbD1, !tbaa !1
  %lnbD2 = ptrtoint %cbCF_str_struct* @cbCF_str$def to i64
  store i64 %lnbD2, i64* %R2_Var
  %lnbD3 = load i64*, i64** %Sp_Var
  %lnbD4 = getelementptr inbounds i64, i64* %lnbD3, i32 -2
  %lnbD5 = ptrtoint i64* %lnbD4 to i64
  %lnbD6 = inttoptr i64 %lnbD5 to i64*
  store i64* %lnbD6, i64** %Sp_Var
  %lnbD7 = bitcast i8* @ghczmprim_GHCziCString_unpackCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbD8 = load i64*, i64** %Sp_Var
  %lnbD9 = load i64, i64* %R1_Var
  %lnbDa = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbD7( i64* %Base_Arg, i64* %lnbD8, i64* %Hp_Arg, i64 %lnbD9, i64 %lnbDa, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbCE:
  %lnbDc = load i64, i64* %R1_Var
  %lnbDd = inttoptr i64 %lnbDc to i64*
  %lnbDe = load i64, i64* %lnbDd, !tbaa !3
  %lnbDf = inttoptr i64 %lnbDe to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbDg = load i64*, i64** %Sp_Var
  %lnbDh = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbDf( i64* %Base_Arg, i64* %lnbDg, i64* %Hp_Arg, i64 %lnbDh, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbCH:
  %lnbDi = load i64, i64* %R1_Var
  store i64 %lnbDi, i64* %R1_Var
  %lnbDj = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lnbDk = bitcast i64* %lnbDj to i64*
  %lnbDl = load i64, i64* %lnbDk, !tbaa !4
  %lnbDm = inttoptr i64 %lnbDl to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbDn = load i64*, i64** %Sp_Var
  %lnbDo = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbDm( i64* %Base_Arg, i64* %lnbDn, i64* %Hp_Arg, i64 %lnbDo, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%rb0u_closure_struct = type <{i64, i64, i64, i64}>
%cbDw_str_struct = type <{[14 x i8]}>
@rb0u_closure$def = internal global %rb0u_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rb0u_info$def to i64), i64 0, i64 0, i64 0}>
@rb0u_closure = internal alias i8* bitcast (%rb0u_closure_struct* @rb0u_closure$def to i8*)
@cbDw_str$def = internal constant %cbDw_str_struct<{[14 x i8] [i8 104, i8 115, i8 108, i8 105, i8 98, i8 47, i8 68, i8 105, i8 115, i8 107, i8 46, i8 104, i8 115, i8 0]}>
@cbDw_str = internal alias i8* bitcast (%cbDw_str_struct* @cbDw_str$def to i8*)
@rb0u_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rb0u_info$def to i8*)
define internal ghccc void @rb0u_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 22}>
{
cbDx:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %lcbDt = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbDA = load i64*, i64** %Sp_Var
  %lnbDB = getelementptr inbounds i64, i64* %lnbDA, i32 -2
  %lnbDC = ptrtoint i64* %lnbDB to i64
  %lnbDD = icmp ult i64 %lnbDC, %SpLim_Arg
  br i1 %lnbDD, label %cbDy, label %cbDz
cbDz:
  %lnbDE = ptrtoint i64* %Base_Arg to i64
  %lnbDF = inttoptr i64 %lnbDE to i8*
  %lnbDG = load i64, i64* %R1_Var
  %lnbDH = inttoptr i64 %lnbDG to i8*
  %lnbDI = bitcast i8* @newCAF to i8* (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lnbDJ = call ccc i8* (i8*, i8*) %lnbDI( i8* %lnbDF, i8* %lnbDH ) nounwind
  %lnbDK = ptrtoint i8* %lnbDJ to i64
  store i64 %lnbDK, i64* %lcbDt
  %lnbDL = load i64, i64* %lcbDt
  %lnbDM = icmp eq i64 %lnbDL, 0
  br i1 %lnbDM, label %cbDv, label %cbDu
cbDu:
  %lnbDO = ptrtoint i8* @stg_bh_upd_frame_info to i64
  %lnbDN = load i64*, i64** %Sp_Var
  %lnbDP = getelementptr inbounds i64, i64* %lnbDN, i32 -2
  store i64 %lnbDO, i64* %lnbDP, !tbaa !1
  %lnbDR = load i64, i64* %lcbDt
  %lnbDQ = load i64*, i64** %Sp_Var
  %lnbDS = getelementptr inbounds i64, i64* %lnbDQ, i32 -1
  store i64 %lnbDR, i64* %lnbDS, !tbaa !1
  %lnbDT = ptrtoint %cbDw_str_struct* @cbDw_str$def to i64
  store i64 %lnbDT, i64* %R2_Var
  %lnbDU = load i64*, i64** %Sp_Var
  %lnbDV = getelementptr inbounds i64, i64* %lnbDU, i32 -2
  %lnbDW = ptrtoint i64* %lnbDV to i64
  %lnbDX = inttoptr i64 %lnbDW to i64*
  store i64* %lnbDX, i64** %Sp_Var
  %lnbDY = bitcast i8* @ghczmprim_GHCziCString_unpackCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbDZ = load i64*, i64** %Sp_Var
  %lnbE0 = load i64, i64* %R1_Var
  %lnbE1 = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbDY( i64* %Base_Arg, i64* %lnbDZ, i64* %Hp_Arg, i64 %lnbE0, i64 %lnbE1, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbDv:
  %lnbE3 = load i64, i64* %R1_Var
  %lnbE4 = inttoptr i64 %lnbE3 to i64*
  %lnbE5 = load i64, i64* %lnbE4, !tbaa !3
  %lnbE6 = inttoptr i64 %lnbE5 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbE7 = load i64*, i64** %Sp_Var
  %lnbE8 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbE6( i64* %Base_Arg, i64* %lnbE7, i64* %Hp_Arg, i64 %lnbE8, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbDy:
  %lnbE9 = load i64, i64* %R1_Var
  store i64 %lnbE9, i64* %R1_Var
  %lnbEa = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lnbEb = bitcast i64* %lnbEa to i64*
  %lnbEc = load i64, i64* %lnbEb, !tbaa !4
  %lnbEd = inttoptr i64 %lnbEc to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbEe = load i64*, i64** %Sp_Var
  %lnbEf = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbEd( i64* %Base_Arg, i64* %lnbEe, i64* %Hp_Arg, i64 %lnbEf, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%rb0v_closure_struct = type <{i64, i64}>
@rb0v_closure$def = internal global %rb0v_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_Izh_static_info to i64), i64 114}>
@rb0v_closure = internal alias i8* bitcast (%rb0v_closure_struct* @rb0v_closure$def to i8*)
%rb0w_closure_struct = type <{i64, i64}>
@rb0w_closure$def = internal global %rb0w_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_Izh_static_info to i64), i64 9}>
@rb0w_closure = internal alias i8* bitcast (%rb0w_closure_struct* @rb0w_closure$def to i8*)
%rb0x_closure_struct = type <{i64, i64}>
@rb0x_closure$def = internal global %rb0x_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_Izh_static_info to i64), i64 78}>
@rb0x_closure = internal alias i8* bitcast (%rb0x_closure_struct* @rb0x_closure$def to i8*)
%rb0y_closure_struct = type <{i64, i64, i64, i64}>
%cbEq_str_struct = type <{[6 x i8]}>
@rb0y_closure$def = internal global %rb0y_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rb0y_info$def to i64), i64 0, i64 0, i64 0}>
@rb0y_closure = internal alias i8* bitcast (%rb0y_closure_struct* @rb0y_closure$def to i8*)
@cbEq_str$def = internal constant %cbEq_str_struct<{[6 x i8] [i8 101, i8 114, i8 114, i8 111, i8 114, i8 0]}>
@cbEq_str = internal alias i8* bitcast (%cbEq_str_struct* @cbEq_str$def to i8*)
@rb0y_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rb0y_info$def to i8*)
define internal ghccc void @rb0y_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 22}>
{
cbEr:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %lcbEn = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbEu = load i64*, i64** %Sp_Var
  %lnbEv = getelementptr inbounds i64, i64* %lnbEu, i32 -2
  %lnbEw = ptrtoint i64* %lnbEv to i64
  %lnbEx = icmp ult i64 %lnbEw, %SpLim_Arg
  br i1 %lnbEx, label %cbEs, label %cbEt
cbEt:
  %lnbEy = ptrtoint i64* %Base_Arg to i64
  %lnbEz = inttoptr i64 %lnbEy to i8*
  %lnbEA = load i64, i64* %R1_Var
  %lnbEB = inttoptr i64 %lnbEA to i8*
  %lnbEC = bitcast i8* @newCAF to i8* (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lnbED = call ccc i8* (i8*, i8*) %lnbEC( i8* %lnbEz, i8* %lnbEB ) nounwind
  %lnbEE = ptrtoint i8* %lnbED to i64
  store i64 %lnbEE, i64* %lcbEn
  %lnbEF = load i64, i64* %lcbEn
  %lnbEG = icmp eq i64 %lnbEF, 0
  br i1 %lnbEG, label %cbEp, label %cbEo
cbEo:
  %lnbEI = ptrtoint i8* @stg_bh_upd_frame_info to i64
  %lnbEH = load i64*, i64** %Sp_Var
  %lnbEJ = getelementptr inbounds i64, i64* %lnbEH, i32 -2
  store i64 %lnbEI, i64* %lnbEJ, !tbaa !1
  %lnbEL = load i64, i64* %lcbEn
  %lnbEK = load i64*, i64** %Sp_Var
  %lnbEM = getelementptr inbounds i64, i64* %lnbEK, i32 -1
  store i64 %lnbEL, i64* %lnbEM, !tbaa !1
  %lnbEN = ptrtoint %cbEq_str_struct* @cbEq_str$def to i64
  store i64 %lnbEN, i64* %R2_Var
  %lnbEO = load i64*, i64** %Sp_Var
  %lnbEP = getelementptr inbounds i64, i64* %lnbEO, i32 -2
  %lnbEQ = ptrtoint i64* %lnbEP to i64
  %lnbER = inttoptr i64 %lnbEQ to i64*
  store i64* %lnbER, i64** %Sp_Var
  %lnbES = bitcast i8* @ghczmprim_GHCziCString_unpackCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbET = load i64*, i64** %Sp_Var
  %lnbEU = load i64, i64* %R1_Var
  %lnbEV = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbES( i64* %Base_Arg, i64* %lnbET, i64* %Hp_Arg, i64 %lnbEU, i64 %lnbEV, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbEp:
  %lnbEX = load i64, i64* %R1_Var
  %lnbEY = inttoptr i64 %lnbEX to i64*
  %lnbEZ = load i64, i64* %lnbEY, !tbaa !3
  %lnbF0 = inttoptr i64 %lnbEZ to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbF1 = load i64*, i64** %Sp_Var
  %lnbF2 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbF0( i64* %Base_Arg, i64* %lnbF1, i64* %Hp_Arg, i64 %lnbF2, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbEs:
  %lnbF3 = load i64, i64* %R1_Var
  store i64 %lnbF3, i64* %R1_Var
  %lnbF4 = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lnbF5 = bitcast i64* %lnbF4 to i64*
  %lnbF6 = load i64, i64* %lnbF5, !tbaa !4
  %lnbF7 = inttoptr i64 %lnbF6 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbF8 = load i64*, i64** %Sp_Var
  %lnbF9 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbF7( i64* %Base_Arg, i64* %lnbF8, i64* %Hp_Arg, i64 %lnbF9, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%rb0z_closure_struct = type <{i64, i64, i64, i64, i64, i64, i64, i64, i64}>
@rb0z_closure$def = internal global %rb0z_closure_struct<{i64 ptrtoint (i8* @base_GHCziStackziTypes_SrcLoc_static_info to i64), i64 ptrtoint (%rb0s_closure_struct* @rb0s_closure$def to i64), i64 ptrtoint (%rb0t_closure_struct* @rb0t_closure$def to i64), i64 ptrtoint (%rb0u_closure_struct* @rb0u_closure$def to i64), i64 add (i64 ptrtoint (%rb0v_closure_struct* @rb0v_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%rb0w_closure_struct* @rb0w_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%rb0v_closure_struct* @rb0v_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%rb0x_closure_struct* @rb0x_closure$def to i64),i64 1), i64 0}>
@rb0z_closure = internal alias i8* bitcast (%rb0z_closure_struct* @rb0z_closure$def to i8*)
%rb0A_closure_struct = type <{i64, i64, i64, i64, i64}>
@rb0A_closure$def = internal global %rb0A_closure_struct<{i64 ptrtoint (i8* @base_GHCziStackziTypes_PushCallStack_static_info to i64), i64 ptrtoint (%rb0y_closure_struct* @rb0y_closure$def to i64), i64 add (i64 ptrtoint (%rb0z_closure_struct* @rb0z_closure$def to i64),i64 1), i64 add (i64 ptrtoint (i8* @base_GHCziStackziTypes_EmptyCallStack_closure to i64),i64 1), i64 0}>
@rb0A_closure = internal alias i8* bitcast (%rb0A_closure_struct* @rb0A_closure$def to i8*)
%rb0B_closure_struct = type <{i64, i64}>
@rb0B_closure$def = internal global %rb0B_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_Izh_static_info to i64), i64 91}>
@rb0B_closure = internal alias i8* bitcast (%rb0B_closure_struct* @rb0B_closure$def to i8*)
%rb0C_closure_struct = type <{i64, i64}>
@rb0C_closure$def = internal global %rb0C_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_Izh_static_info to i64), i64 76}>
@rb0C_closure = internal alias i8* bitcast (%rb0C_closure_struct* @rb0C_closure$def to i8*)
%rb0D_closure_struct = type <{i64, i64, i64, i64, i64, i64, i64, i64, i64}>
@rb0D_closure$def = internal global %rb0D_closure_struct<{i64 ptrtoint (i8* @base_GHCziStackziTypes_SrcLoc_static_info to i64), i64 ptrtoint (%rb0s_closure_struct* @rb0s_closure$def to i64), i64 ptrtoint (%rb0t_closure_struct* @rb0t_closure$def to i64), i64 ptrtoint (%rb0u_closure_struct* @rb0u_closure$def to i64), i64 add (i64 ptrtoint (%rb0B_closure_struct* @rb0B_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%rb0w_closure_struct* @rb0w_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%rb0B_closure_struct* @rb0B_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%rb0C_closure_struct* @rb0C_closure$def to i64),i64 1), i64 0}>
@rb0D_closure = internal alias i8* bitcast (%rb0D_closure_struct* @rb0D_closure$def to i8*)
%rb0E_closure_struct = type <{i64, i64, i64, i64, i64}>
@rb0E_closure$def = internal global %rb0E_closure_struct<{i64 ptrtoint (i8* @base_GHCziStackziTypes_PushCallStack_static_info to i64), i64 ptrtoint (%rb0y_closure_struct* @rb0y_closure$def to i64), i64 add (i64 ptrtoint (%rb0D_closure_struct* @rb0D_closure$def to i64),i64 1), i64 add (i64 ptrtoint (i8* @base_GHCziStackziTypes_EmptyCallStack_closure to i64),i64 1), i64 0}>
@rb0E_closure = internal alias i8* bitcast (%rb0E_closure_struct* @rb0E_closure$def to i8*)
%Disk_varzualloc2_closure_struct = type <{i64, i64}>
@Disk_varzualloc2_closure$def = internal global %Disk_varzualloc2_closure_struct<{i64 ptrtoint (i8* @integerzmgmp_GHCziIntegerziType_Szh_static_info to i64), i64 1}>
@Disk_varzualloc2_closure = alias i8* bitcast (%Disk_varzualloc2_closure_struct* @Disk_varzualloc2_closure$def to i8*)
%Disk_zdwvarzualloc_closure_struct = type <{i64, i64}>
@Disk_zdwvarzualloc_closure$def = internal global %Disk_zdwvarzualloc_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwvarzualloc_info$def to i64), i64 0}>
@Disk_zdwvarzualloc_closure = alias i8* bitcast (%Disk_zdwvarzualloc_closure_struct* @Disk_zdwvarzualloc_closure$def to i8*)
@sb2d_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb2d_info$def to i8*)
define internal ghccc void @sb2d_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb2d_info$def to i64)),i64 40), i64 1, i64 4294967313}>
{
cbFF:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbG0 = load i64*, i64** %Sp_Var
  %lnbG1 = getelementptr inbounds i64, i64* %lnbG0, i32 -2
  %lnbG2 = ptrtoint i64* %lnbG1 to i64
  %lnbG3 = icmp ult i64 %lnbG2, %SpLim_Arg
  br i1 %lnbG3, label %cbFG, label %cbFH
cbFH:
  %lnbG5 = ptrtoint i8* @stg_upd_frame_info to i64
  %lnbG4 = load i64*, i64** %Sp_Var
  %lnbG6 = getelementptr inbounds i64, i64* %lnbG4, i32 -2
  store i64 %lnbG5, i64* %lnbG6, !tbaa !1
  %lnbG8 = load i64, i64* %R1_Var
  %lnbG7 = load i64*, i64** %Sp_Var
  %lnbG9 = getelementptr inbounds i64, i64* %lnbG7, i32 -1
  store i64 %lnbG8, i64* %lnbG9, !tbaa !1
  %lnbGa = ptrtoint %Disk_varzualloc2_closure_struct* @Disk_varzualloc2_closure$def to i64
  %lnbGb = add i64 %lnbGa, 1
  store i64 %lnbGb, i64* %R3_Var
  %lnbGe = load i64, i64* %R1_Var
  %lnbGf = add i64 %lnbGe, 16
  %lnbGg = inttoptr i64 %lnbGf to i64*
  %lnbGh = load i64, i64* %lnbGg, !tbaa !3
  store i64 %lnbGh, i64* %R2_Var
  %lnbGi = load i64*, i64** %Sp_Var
  %lnbGj = getelementptr inbounds i64, i64* %lnbGi, i32 -2
  %lnbGk = ptrtoint i64* %lnbGj to i64
  %lnbGl = inttoptr i64 %lnbGk to i64*
  store i64* %lnbGl, i64** %Sp_Var
  %lnbGm = bitcast i8* @integerzmgmp_GHCziIntegerziType_plusInteger_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbGn = load i64*, i64** %Sp_Var
  %lnbGo = load i64, i64* %R1_Var
  %lnbGp = load i64, i64* %R2_Var
  %lnbGq = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbGm( i64* %Base_Arg, i64* %lnbGn, i64* %Hp_Arg, i64 %lnbGo, i64 %lnbGp, i64 %lnbGq, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbFG:
  %lnbGr = load i64, i64* %R1_Var
  store i64 %lnbGr, i64* %R1_Var
  %lnbGs = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lnbGt = bitcast i64* %lnbGs to i64*
  %lnbGu = load i64, i64* %lnbGt, !tbaa !4
  %lnbGv = inttoptr i64 %lnbGu to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbGw = load i64*, i64** %Sp_Var
  %lnbGx = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbGv( i64* %Base_Arg, i64* %lnbGw, i64* %Hp_Arg, i64 %lnbGx, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@sb2c_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb2c_info$def to i8*)
define internal ghccc void @sb2c_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb2c_info$def to i64)),i64 32), i64 3, i64 4294967312}>
{
cbFM:
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbGy = load i64*, i64** %Sp_Var
  %lnbGz = getelementptr inbounds i64, i64* %lnbGy, i32 -2
  %lnbGA = ptrtoint i64* %lnbGz to i64
  %lnbGB = icmp ult i64 %lnbGA, %SpLim_Arg
  br i1 %lnbGB, label %cbFN, label %cbFO
cbFO:
  %lnbGD = ptrtoint i8* @stg_upd_frame_info to i64
  %lnbGC = load i64*, i64** %Sp_Var
  %lnbGE = getelementptr inbounds i64, i64* %lnbGC, i32 -2
  store i64 %lnbGD, i64* %lnbGE, !tbaa !1
  %lnbGG = load i64, i64* %R1_Var
  %lnbGF = load i64*, i64** %Sp_Var
  %lnbGH = getelementptr inbounds i64, i64* %lnbGF, i32 -1
  store i64 %lnbGG, i64* %lnbGH, !tbaa !1
  %lnbGK = load i64, i64* %R1_Var
  %lnbGL = add i64 %lnbGK, 24
  %lnbGM = inttoptr i64 %lnbGL to i64*
  %lnbGN = load i64, i64* %lnbGM, !tbaa !3
  store i64 %lnbGN, i64* %R4_Var
  %lnbGQ = load i64, i64* %R1_Var
  %lnbGR = add i64 %lnbGQ, 16
  %lnbGS = inttoptr i64 %lnbGR to i64*
  %lnbGT = load i64, i64* %lnbGS, !tbaa !3
  store i64 %lnbGT, i64* %R3_Var
  %lnbGW = load i64, i64* %R1_Var
  %lnbGX = add i64 %lnbGW, 32
  %lnbGY = inttoptr i64 %lnbGX to i64*
  %lnbGZ = load i64, i64* %lnbGY, !tbaa !3
  store i64 %lnbGZ, i64* %R2_Var
  %lnbH0 = load i64*, i64** %Sp_Var
  %lnbH1 = getelementptr inbounds i64, i64* %lnbH0, i32 -2
  %lnbH2 = ptrtoint i64* %lnbH1 to i64
  %lnbH3 = inttoptr i64 %lnbH2 to i64*
  store i64* %lnbH3, i64** %Sp_Var
  %lnbH4 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdsinsertzuzdsgo4_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbH5 = load i64*, i64** %Sp_Var
  %lnbH6 = load i64, i64* %R1_Var
  %lnbH7 = load i64, i64* %R2_Var
  %lnbH8 = load i64, i64* %R3_Var
  %lnbH9 = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbH4( i64* %Base_Arg, i64* %lnbH5, i64* %Hp_Arg, i64 %lnbH6, i64 %lnbH7, i64 %lnbH8, i64 %lnbH9, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbFN:
  %lnbHa = load i64, i64* %R1_Var
  store i64 %lnbHa, i64* %R1_Var
  %lnbHb = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lnbHc = bitcast i64* %lnbHb to i64*
  %lnbHd = load i64, i64* %lnbHc, !tbaa !4
  %lnbHe = inttoptr i64 %lnbHd to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbHf = load i64*, i64** %Sp_Var
  %lnbHg = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbHe( i64* %Base_Arg, i64* %lnbHf, i64* %Hp_Arg, i64 %lnbHg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@Disk_zdwvarzualloc_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwvarzualloc_info$def to i8*)
define ghccc void @Disk_zdwvarzualloc_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwvarzualloc_info$def to i64)),i64 32), i64 17179869207, i64 0, i64 30064771087}>
{
cbFP:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R4_Var = alloca i64, i32 1
  store i64 %R4_Arg, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnbHh = load i64*, i64** %Sp_Var
  %lnbHi = getelementptr inbounds i64, i64* %lnbHh, i32 -3
  %lnbHj = ptrtoint i64* %lnbHi to i64
  %lnbHk = icmp ult i64 %lnbHj, %SpLim_Arg
  br i1 %lnbHk, label %cbFQ, label %cbFR
cbFR:
  %lnbHm = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbFw_info$def to i64
  %lnbHl = load i64*, i64** %Sp_Var
  %lnbHn = getelementptr inbounds i64, i64* %lnbHl, i32 -3
  store i64 %lnbHm, i64* %lnbHn, !tbaa !1
  %lnbHq = load i64, i64* %R3_Var
  %lnbHr = add i64 %lnbHq, 8
  %lnbHs = inttoptr i64 %lnbHr to i64*
  %lnbHt = load i64, i64* %lnbHs, !tbaa !3
  store i64 %lnbHt, i64* %R1_Var
  %lnbHv = load i64, i64* %R3_Var
  %lnbHu = load i64*, i64** %Sp_Var
  %lnbHw = getelementptr inbounds i64, i64* %lnbHu, i32 -2
  store i64 %lnbHv, i64* %lnbHw, !tbaa !1
  %lnbHy = load i64, i64* %R4_Var
  %lnbHx = load i64*, i64** %Sp_Var
  %lnbHz = getelementptr inbounds i64, i64* %lnbHx, i32 -1
  store i64 %lnbHy, i64* %lnbHz, !tbaa !1
  %lnbHA = load i64*, i64** %Sp_Var
  %lnbHB = getelementptr inbounds i64, i64* %lnbHA, i32 -3
  %lnbHC = ptrtoint i64* %lnbHB to i64
  %lnbHD = inttoptr i64 %lnbHC to i64*
  store i64* %lnbHD, i64** %Sp_Var
  %lnbHE = load i64, i64* %R1_Var
  %lnbHF = and i64 %lnbHE, 7
  %lnbHG = icmp ne i64 %lnbHF, 0
  br i1 %lnbHG, label %ubFZ, label %cbFx
cbFx:
  %lnbHI = load i64, i64* %R1_Var
  %lnbHJ = inttoptr i64 %lnbHI to i64*
  %lnbHK = load i64, i64* %lnbHJ, !tbaa !3
  %lnbHL = inttoptr i64 %lnbHK to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbHM = load i64*, i64** %Sp_Var
  %lnbHN = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbHL( i64* %Base_Arg, i64* %lnbHM, i64* %Hp_Arg, i64 %lnbHN, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubFZ:
  %lnbHO = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbFw_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbHP = load i64*, i64** %Sp_Var
  %lnbHQ = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbHO( i64* %Base_Arg, i64* %lnbHP, i64* %Hp_Arg, i64 %lnbHQ, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbFQ:
  %lnbHR = load i64, i64* %R4_Var
  store i64 %lnbHR, i64* %R4_Var
  %lnbHS = load i64, i64* %R3_Var
  store i64 %lnbHS, i64* %R3_Var
  %lnbHT = load i64, i64* %R2_Var
  store i64 %lnbHT, i64* %R2_Var
  %lnbHU = ptrtoint %Disk_zdwvarzualloc_closure_struct* @Disk_zdwvarzualloc_closure$def to i64
  store i64 %lnbHU, i64* %R1_Var
  %lnbHV = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lnbHW = bitcast i64* %lnbHV to i64*
  %lnbHX = load i64, i64* %lnbHW, !tbaa !4
  %lnbHY = inttoptr i64 %lnbHX to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbHZ = load i64*, i64** %Sp_Var
  %lnbI0 = load i64, i64* %R1_Var
  %lnbI1 = load i64, i64* %R2_Var
  %lnbI2 = load i64, i64* %R3_Var
  %lnbI3 = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbHY( i64* %Base_Arg, i64* %lnbHZ, i64* %Hp_Arg, i64 %lnbI0, i64 %lnbI1, i64 %lnbI2, i64 %lnbI3, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbFw_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbFw_info$def to i8*)
define internal ghccc void @cbFw_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbFw_info$def to i64)),i64 32), i64 2, i64 12884901920}>
{
cbFw:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb2a = alloca i64, i32 1
  %lsb2b = alloca i64, i32 1
  %lsb23 = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnbI4 = load i64*, i64** %Hp_Var
  %lnbI5 = getelementptr inbounds i64, i64* %lnbI4, i32 11
  %lnbI6 = ptrtoint i64* %lnbI5 to i64
  %lnbI7 = inttoptr i64 %lnbI6 to i64*
  store i64* %lnbI7, i64** %Hp_Var
  %lnbI8 = load i64*, i64** %Hp_Var
  %lnbI9 = ptrtoint i64* %lnbI8 to i64
  %lnbIa = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnbIb = bitcast i64* %lnbIa to i64*
  %lnbIc = load i64, i64* %lnbIb, !tbaa !4
  %lnbId = icmp ugt i64 %lnbI9, %lnbIc
  br i1 %lnbId, label %cbFU, label %cbFT
cbFT:
  %lnbIg = load i64, i64* %R1_Var
  %lnbIh = add i64 %lnbIg, 7
  %lnbIi = inttoptr i64 %lnbIh to i64*
  %lnbIj = load i64, i64* %lnbIi, !tbaa !3
  store i64 %lnbIj, i64* %lsb2a
  %lnbIm = load i64, i64* %R1_Var
  %lnbIn = add i64 %lnbIm, 15
  %lnbIo = inttoptr i64 %lnbIn to i64*
  %lnbIp = load i64, i64* %lnbIo, !tbaa !3
  store i64 %lnbIp, i64* %lsb2b
  %lnbIr = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb2d_info$def to i64
  %lnbIq = load i64*, i64** %Hp_Var
  %lnbIs = getelementptr inbounds i64, i64* %lnbIq, i32 -10
  store i64 %lnbIr, i64* %lnbIs, !tbaa !2
  %lnbIu = load i64, i64* %lsb2b
  %lnbIt = load i64*, i64** %Hp_Var
  %lnbIv = getelementptr inbounds i64, i64* %lnbIt, i32 -8
  store i64 %lnbIu, i64* %lnbIv, !tbaa !2
  %lnbIx = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb2c_info$def to i64
  %lnbIw = load i64*, i64** %Hp_Var
  %lnbIy = getelementptr inbounds i64, i64* %lnbIw, i32 -7
  store i64 %lnbIx, i64* %lnbIy, !tbaa !2
  %lnbIA = load i64*, i64** %Sp_Var
  %lnbIB = getelementptr inbounds i64, i64* %lnbIA, i32 2
  %lnbIC = bitcast i64* %lnbIB to i64*
  %lnbID = load i64, i64* %lnbIC, !tbaa !1
  %lnbIz = load i64*, i64** %Hp_Var
  %lnbIE = getelementptr inbounds i64, i64* %lnbIz, i32 -5
  store i64 %lnbID, i64* %lnbIE, !tbaa !2
  %lnbIG = load i64, i64* %lsb2a
  %lnbIF = load i64*, i64** %Hp_Var
  %lnbIH = getelementptr inbounds i64, i64* %lnbIF, i32 -4
  store i64 %lnbIG, i64* %lnbIH, !tbaa !2
  %lnbIJ = load i64, i64* %lsb2b
  %lnbII = load i64*, i64** %Hp_Var
  %lnbIK = getelementptr inbounds i64, i64* %lnbII, i32 -3
  store i64 %lnbIJ, i64* %lnbIK, !tbaa !2
  %lnbIM = ptrtoint i8* @Disk_VS_con_info to i64
  %lnbIL = load i64*, i64** %Hp_Var
  %lnbIN = getelementptr inbounds i64, i64* %lnbIL, i32 -2
  store i64 %lnbIM, i64* %lnbIN, !tbaa !2
  %lnbIP = load i64*, i64** %Hp_Var
  %lnbIQ = getelementptr inbounds i64, i64* %lnbIP, i32 -7
  %lnbIR = ptrtoint i64* %lnbIQ to i64
  %lnbIO = load i64*, i64** %Hp_Var
  %lnbIS = getelementptr inbounds i64, i64* %lnbIO, i32 -1
  store i64 %lnbIR, i64* %lnbIS, !tbaa !2
  %lnbIU = load i64*, i64** %Hp_Var
  %lnbIV = getelementptr inbounds i64, i64* %lnbIU, i32 -10
  %lnbIW = ptrtoint i64* %lnbIV to i64
  %lnbIT = load i64*, i64** %Hp_Var
  %lnbIX = getelementptr inbounds i64, i64* %lnbIT, i32 0
  store i64 %lnbIW, i64* %lnbIX, !tbaa !2
  %lnbIY = load i64*, i64** %Sp_Var
  %lnbIZ = getelementptr inbounds i64, i64* %lnbIY, i32 1
  %lnbJ0 = bitcast i64* %lnbIZ to i64*
  %lnbJ1 = load i64, i64* %lnbJ0, !tbaa !1
  store i64 %lnbJ1, i64* %lsb23
  %lnbJ2 = load i64, i64* %lsb23
  %lnbJ3 = add i64 %lnbJ2, 8
  %lnbJ5 = load i64*, i64** %Hp_Var
  %lnbJ6 = ptrtoint i64* %lnbJ5 to i64
  %lnbJ7 = add i64 %lnbJ6, -15
  %lnbJ8 = inttoptr i64 %lnbJ3 to i64*
  store i64 %lnbJ7, i64* %lnbJ8, !tbaa !5
  %lnbJ9 = ptrtoint i64* %Base_Arg to i64
  %lnbJa = inttoptr i64 %lnbJ9 to i8*
  %lnbJb = load i64, i64* %lsb23
  %lnbJc = inttoptr i64 %lnbJb to i8*
  %lnbJd = bitcast i8* @dirty_MUT_VAR to void (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  call ccc void (i8*, i8*) %lnbJd( i8* %lnbJa, i8* %lnbJc ) nounwind
  %lnbJe = load i64, i64* %lsb2b
  store i64 %lnbJe, i64* %R1_Var
  %lnbJf = load i64*, i64** %Sp_Var
  %lnbJg = getelementptr inbounds i64, i64* %lnbJf, i32 3
  %lnbJh = ptrtoint i64* %lnbJg to i64
  %lnbJi = inttoptr i64 %lnbJh to i64*
  store i64* %lnbJi, i64** %Sp_Var
  %lnbJj = load i64*, i64** %Sp_Var
  %lnbJk = getelementptr inbounds i64, i64* %lnbJj, i32 0
  %lnbJl = bitcast i64* %lnbJk to i64*
  %lnbJm = load i64, i64* %lnbJl, !tbaa !1
  %lnbJn = inttoptr i64 %lnbJm to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbJo = load i64*, i64** %Sp_Var
  %lnbJp = load i64*, i64** %Hp_Var
  %lnbJq = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbJn( i64* %Base_Arg, i64* %lnbJo, i64* %lnbJp, i64 %lnbJq, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbFU:
  %lnbJr = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 88, i64* %lnbJr, !tbaa !4
  %lnbJs = load i64, i64* %R1_Var
  store i64 %lnbJs, i64* %R1_Var
  %lnbJt = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbJu = load i64*, i64** %Sp_Var
  %lnbJv = load i64*, i64** %Hp_Var
  %lnbJw = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbJt( i64* %Base_Arg, i64* %lnbJu, i64* %lnbJv, i64 %lnbJw, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_varzualloc1_closure_struct = type <{i64, i64}>
@Disk_varzualloc1_closure$def = internal global %Disk_varzualloc1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzualloc1_info$def to i64), i64 0}>
@Disk_varzualloc1_closure = alias i8* bitcast (%Disk_varzualloc1_closure_struct* @Disk_varzualloc1_closure$def to i8*)
@Disk_varzualloc1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzualloc1_info$def to i8*)
define ghccc void @Disk_varzualloc1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzualloc1_info$def to i64)),i64 48), i64 12884901903, i64 0, i64 12884901903}>
{
cbJI:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnbJP = load i64*, i64** %Sp_Var
  %lnbJQ = getelementptr inbounds i64, i64* %lnbJP, i32 -2
  %lnbJR = ptrtoint i64* %lnbJQ to i64
  %lnbJS = icmp ult i64 %lnbJR, %SpLim_Arg
  br i1 %lnbJS, label %cbJJ, label %cbJK
cbJK:
  %lnbJU = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbJF_info$def to i64
  %lnbJT = load i64*, i64** %Sp_Var
  %lnbJV = getelementptr inbounds i64, i64* %lnbJT, i32 -2
  store i64 %lnbJU, i64* %lnbJV, !tbaa !1
  %lnbJW = load i64, i64* %R2_Var
  store i64 %lnbJW, i64* %R1_Var
  %lnbJY = load i64, i64* %R3_Var
  %lnbJX = load i64*, i64** %Sp_Var
  %lnbJZ = getelementptr inbounds i64, i64* %lnbJX, i32 -1
  store i64 %lnbJY, i64* %lnbJZ, !tbaa !1
  %lnbK0 = load i64*, i64** %Sp_Var
  %lnbK1 = getelementptr inbounds i64, i64* %lnbK0, i32 -2
  %lnbK2 = ptrtoint i64* %lnbK1 to i64
  %lnbK3 = inttoptr i64 %lnbK2 to i64*
  store i64* %lnbK3, i64** %Sp_Var
  %lnbK4 = load i64, i64* %R1_Var
  %lnbK5 = and i64 %lnbK4, 7
  %lnbK6 = icmp ne i64 %lnbK5, 0
  br i1 %lnbK6, label %ubJO, label %cbJG
cbJG:
  %lnbK8 = load i64, i64* %R1_Var
  %lnbK9 = inttoptr i64 %lnbK8 to i64*
  %lnbKa = load i64, i64* %lnbK9, !tbaa !3
  %lnbKb = inttoptr i64 %lnbKa to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbKc = load i64*, i64** %Sp_Var
  %lnbKd = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbKb( i64* %Base_Arg, i64* %lnbKc, i64* %Hp_Arg, i64 %lnbKd, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubJO:
  %lnbKe = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbJF_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbKf = load i64*, i64** %Sp_Var
  %lnbKg = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbKe( i64* %Base_Arg, i64* %lnbKf, i64* %Hp_Arg, i64 %lnbKg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbJJ:
  %lnbKh = load i64, i64* %R3_Var
  store i64 %lnbKh, i64* %R3_Var
  %lnbKi = load i64, i64* %R2_Var
  store i64 %lnbKi, i64* %R2_Var
  %lnbKj = ptrtoint %Disk_varzualloc1_closure_struct* @Disk_varzualloc1_closure$def to i64
  store i64 %lnbKj, i64* %R1_Var
  %lnbKk = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lnbKl = bitcast i64* %lnbKk to i64*
  %lnbKm = load i64, i64* %lnbKl, !tbaa !4
  %lnbKn = inttoptr i64 %lnbKm to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbKo = load i64*, i64** %Sp_Var
  %lnbKp = load i64, i64* %R1_Var
  %lnbKq = load i64, i64* %R2_Var
  %lnbKr = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbKn( i64* %Base_Arg, i64* %lnbKo, i64* %Hp_Arg, i64 %lnbKp, i64 %lnbKq, i64 %lnbKr, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbJF_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbJF_info$def to i8*)
define internal ghccc void @cbJF_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbJF_info$def to i64)),i64 48), i64 1, i64 4294967328}>
{
cbJF:
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnbKs = load i64*, i64** %Sp_Var
  %lnbKt = getelementptr inbounds i64, i64* %lnbKs, i32 1
  %lnbKu = bitcast i64* %lnbKt to i64*
  %lnbKv = load i64, i64* %lnbKu, !tbaa !1
  store i64 %lnbKv, i64* %R4_Var
  %lnbKw = add i64 %R1_Arg, 23
  %lnbKx = inttoptr i64 %lnbKw to i64*
  %lnbKy = load i64, i64* %lnbKx, !tbaa !3
  store i64 %lnbKy, i64* %R3_Var
  %lnbKz = add i64 %R1_Arg, 7
  %lnbKA = inttoptr i64 %lnbKz to i64*
  %lnbKB = load i64, i64* %lnbKA, !tbaa !3
  store i64 %lnbKB, i64* %R2_Var
  %lnbKC = load i64*, i64** %Sp_Var
  %lnbKD = getelementptr inbounds i64, i64* %lnbKC, i32 2
  %lnbKE = ptrtoint i64* %lnbKD to i64
  %lnbKF = inttoptr i64 %lnbKE to i64*
  store i64* %lnbKF, i64** %Sp_Var
  %lnbKG = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwvarzualloc_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbKH = load i64*, i64** %Sp_Var
  %lnbKI = load i64, i64* %R2_Var
  %lnbKJ = load i64, i64* %R3_Var
  %lnbKK = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbKG( i64* %Base_Arg, i64* %lnbKH, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnbKI, i64 %lnbKJ, i64 %lnbKK, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_varzualloc_closure_struct = type <{i64, i64}>
@Disk_varzualloc_closure$def = internal global %Disk_varzualloc_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzualloc_info$def to i64), i64 0}>
@Disk_varzualloc_closure = alias i8* bitcast (%Disk_varzualloc_closure_struct* @Disk_varzualloc_closure$def to i8*)
@Disk_varzualloc_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzualloc_info$def to i8*)
define ghccc void @Disk_varzualloc_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzualloc_info$def to i64)),i64 56), i64 12884901903, i64 0, i64 4294967311}>
{
cbKQ:
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnbKT = load i64, i64* %R3_Var
  store i64 %lnbKT, i64* %R3_Var
  %lnbKU = load i64, i64* %R2_Var
  store i64 %lnbKU, i64* %R2_Var
  %lnbKV = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzualloc1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbKW = load i64, i64* %R2_Var
  %lnbKX = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbKV( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnbKW, i64 %lnbKX, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_varzudelete1_closure_struct = type <{i64, i64}>
@Disk_varzudelete1_closure$def = internal global %Disk_varzudelete1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzudelete1_info$def to i64), i64 0}>
@Disk_varzudelete1_closure = alias i8* bitcast (%Disk_varzudelete1_closure_struct* @Disk_varzudelete1_closure$def to i8*)
@sb2C_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb2C_info$def to i8*)
define internal ghccc void @sb2C_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb2C_info$def to i64)),i64 24), i64 2, i64 4294967315}>
{
cbLt:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbLJ = load i64*, i64** %Sp_Var
  %lnbLK = getelementptr inbounds i64, i64* %lnbLJ, i32 -2
  %lnbLL = ptrtoint i64* %lnbLK to i64
  %lnbLM = icmp ult i64 %lnbLL, %SpLim_Arg
  br i1 %lnbLM, label %cbLu, label %cbLv
cbLv:
  %lnbLO = ptrtoint i8* @stg_upd_frame_info to i64
  %lnbLN = load i64*, i64** %Sp_Var
  %lnbLP = getelementptr inbounds i64, i64* %lnbLN, i32 -2
  store i64 %lnbLO, i64* %lnbLP, !tbaa !1
  %lnbLR = load i64, i64* %R1_Var
  %lnbLQ = load i64*, i64** %Sp_Var
  %lnbLS = getelementptr inbounds i64, i64* %lnbLQ, i32 -1
  store i64 %lnbLR, i64* %lnbLS, !tbaa !1
  %lnbLV = load i64, i64* %R1_Var
  %lnbLW = add i64 %lnbLV, 24
  %lnbLX = inttoptr i64 %lnbLW to i64*
  %lnbLY = load i64, i64* %lnbLX, !tbaa !3
  store i64 %lnbLY, i64* %R3_Var
  %lnbM1 = load i64, i64* %R1_Var
  %lnbM2 = add i64 %lnbM1, 16
  %lnbM3 = inttoptr i64 %lnbM2 to i64*
  %lnbM4 = load i64, i64* %lnbM3, !tbaa !3
  store i64 %lnbM4, i64* %R2_Var
  %lnbM5 = load i64*, i64** %Sp_Var
  %lnbM6 = getelementptr inbounds i64, i64* %lnbM5, i32 -2
  %lnbM7 = ptrtoint i64* %lnbM6 to i64
  %lnbM8 = inttoptr i64 %lnbM7 to i64*
  store i64* %lnbM8, i64** %Sp_Var
  %lnbM9 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdsdeletezuzdsgo10_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbMa = load i64*, i64** %Sp_Var
  %lnbMb = load i64, i64* %R1_Var
  %lnbMc = load i64, i64* %R2_Var
  %lnbMd = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbM9( i64* %Base_Arg, i64* %lnbMa, i64* %Hp_Arg, i64 %lnbMb, i64 %lnbMc, i64 %lnbMd, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbLu:
  %lnbMe = load i64, i64* %R1_Var
  store i64 %lnbMe, i64* %R1_Var
  %lnbMf = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lnbMg = bitcast i64* %lnbMf to i64*
  %lnbMh = load i64, i64* %lnbMg, !tbaa !4
  %lnbMi = inttoptr i64 %lnbMh to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbMj = load i64*, i64** %Sp_Var
  %lnbMk = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbMi( i64* %Base_Arg, i64* %lnbMj, i64* %Hp_Arg, i64 %lnbMk, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@Disk_varzudelete1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzudelete1_info$def to i8*)
define ghccc void @Disk_varzudelete1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzudelete1_info$def to i64)),i64 24), i64 12884901903, i64 0, i64 141733920783}>
{
cbLw:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnbMl = load i64*, i64** %Sp_Var
  %lnbMm = getelementptr inbounds i64, i64* %lnbMl, i32 -3
  %lnbMn = ptrtoint i64* %lnbMm to i64
  %lnbMo = icmp ult i64 %lnbMn, %SpLim_Arg
  br i1 %lnbMo, label %cbLx, label %cbLy
cbLy:
  %lnbMq = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbLf_info$def to i64
  %lnbMp = load i64*, i64** %Sp_Var
  %lnbMr = getelementptr inbounds i64, i64* %lnbMp, i32 -2
  store i64 %lnbMq, i64* %lnbMr, !tbaa !1
  %lnbMs = load i64, i64* %R2_Var
  store i64 %lnbMs, i64* %R1_Var
  %lnbMu = load i64, i64* %R3_Var
  %lnbMt = load i64*, i64** %Sp_Var
  %lnbMv = getelementptr inbounds i64, i64* %lnbMt, i32 -1
  store i64 %lnbMu, i64* %lnbMv, !tbaa !1
  %lnbMw = load i64*, i64** %Sp_Var
  %lnbMx = getelementptr inbounds i64, i64* %lnbMw, i32 -2
  %lnbMy = ptrtoint i64* %lnbMx to i64
  %lnbMz = inttoptr i64 %lnbMy to i64*
  store i64* %lnbMz, i64** %Sp_Var
  %lnbMA = load i64, i64* %R1_Var
  %lnbMB = and i64 %lnbMA, 7
  %lnbMC = icmp ne i64 %lnbMB, 0
  br i1 %lnbMC, label %ubLI, label %cbLg
cbLg:
  %lnbME = load i64, i64* %R1_Var
  %lnbMF = inttoptr i64 %lnbME to i64*
  %lnbMG = load i64, i64* %lnbMF, !tbaa !3
  %lnbMH = inttoptr i64 %lnbMG to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbMI = load i64*, i64** %Sp_Var
  %lnbMJ = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbMH( i64* %Base_Arg, i64* %lnbMI, i64* %Hp_Arg, i64 %lnbMJ, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubLI:
  %lnbMK = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbLf_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbML = load i64*, i64** %Sp_Var
  %lnbMM = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbMK( i64* %Base_Arg, i64* %lnbML, i64* %Hp_Arg, i64 %lnbMM, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbLx:
  %lnbMN = load i64, i64* %R3_Var
  store i64 %lnbMN, i64* %R3_Var
  %lnbMO = load i64, i64* %R2_Var
  store i64 %lnbMO, i64* %R2_Var
  %lnbMP = ptrtoint %Disk_varzudelete1_closure_struct* @Disk_varzudelete1_closure$def to i64
  store i64 %lnbMP, i64* %R1_Var
  %lnbMQ = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lnbMR = bitcast i64* %lnbMQ to i64*
  %lnbMS = load i64, i64* %lnbMR, !tbaa !4
  %lnbMT = inttoptr i64 %lnbMS to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbMU = load i64*, i64** %Sp_Var
  %lnbMV = load i64, i64* %R1_Var
  %lnbMW = load i64, i64* %R2_Var
  %lnbMX = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbMT( i64* %Base_Arg, i64* %lnbMU, i64* %Hp_Arg, i64 %lnbMV, i64 %lnbMW, i64 %lnbMX, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbLf_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbLf_info$def to i8*)
define internal ghccc void @cbLf_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbLf_info$def to i64)),i64 24), i64 1, i64 4294967328}>
{
cbLf:
  %lsb2v = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnbMZ = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbLk_info$def to i64
  %lnbMY = load i64*, i64** %Sp_Var
  %lnbN0 = getelementptr inbounds i64, i64* %lnbMY, i32 -1
  store i64 %lnbMZ, i64* %lnbN0, !tbaa !1
  %lnbN3 = load i64, i64* %R1_Var
  %lnbN4 = add i64 %lnbN3, 23
  %lnbN5 = inttoptr i64 %lnbN4 to i64*
  %lnbN6 = load i64, i64* %lnbN5, !tbaa !3
  store i64 %lnbN6, i64* %lsb2v
  %lnbN7 = load i64, i64* %lsb2v
  %lnbN8 = add i64 %lnbN7, 8
  %lnbN9 = inttoptr i64 %lnbN8 to i64*
  %lnbNa = load i64, i64* %lnbN9, !tbaa !5
  store i64 %lnbNa, i64* %R1_Var
  %lnbNc = load i64, i64* %lsb2v
  %lnbNb = load i64*, i64** %Sp_Var
  %lnbNd = getelementptr inbounds i64, i64* %lnbNb, i32 0
  store i64 %lnbNc, i64* %lnbNd, !tbaa !1
  %lnbNe = load i64*, i64** %Sp_Var
  %lnbNf = getelementptr inbounds i64, i64* %lnbNe, i32 -1
  %lnbNg = ptrtoint i64* %lnbNf to i64
  %lnbNh = inttoptr i64 %lnbNg to i64*
  store i64* %lnbNh, i64** %Sp_Var
  %lnbNi = load i64, i64* %R1_Var
  %lnbNj = and i64 %lnbNi, 7
  %lnbNk = icmp ne i64 %lnbNj, 0
  br i1 %lnbNk, label %ubLH, label %cbLl
cbLl:
  %lnbNm = load i64, i64* %R1_Var
  %lnbNn = inttoptr i64 %lnbNm to i64*
  %lnbNo = load i64, i64* %lnbNn, !tbaa !3
  %lnbNp = inttoptr i64 %lnbNo to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbNq = load i64*, i64** %Sp_Var
  %lnbNr = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbNp( i64* %Base_Arg, i64* %lnbNq, i64* %Hp_Arg, i64 %lnbNr, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubLH:
  %lnbNs = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbLk_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbNt = load i64*, i64** %Sp_Var
  %lnbNu = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbNs( i64* %Base_Arg, i64* %lnbNt, i64* %Hp_Arg, i64 %lnbNu, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbLk_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbLk_info$def to i8*)
define internal ghccc void @cbLk_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbLk_info$def to i64)),i64 24), i64 2, i64 4294967328}>
{
cbLk:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb2A = alloca i64, i32 1
  %lsb2B = alloca i64, i32 1
  %lsb2v = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnbNv = load i64*, i64** %Hp_Var
  %lnbNw = getelementptr inbounds i64, i64* %lnbNv, i32 7
  %lnbNx = ptrtoint i64* %lnbNw to i64
  %lnbNy = inttoptr i64 %lnbNx to i64*
  store i64* %lnbNy, i64** %Hp_Var
  %lnbNz = load i64*, i64** %Hp_Var
  %lnbNA = ptrtoint i64* %lnbNz to i64
  %lnbNB = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnbNC = bitcast i64* %lnbNB to i64*
  %lnbND = load i64, i64* %lnbNC, !tbaa !4
  %lnbNE = icmp ugt i64 %lnbNA, %lnbND
  br i1 %lnbNE, label %cbLC, label %cbLB
cbLB:
  %lnbNH = load i64, i64* %R1_Var
  %lnbNI = add i64 %lnbNH, 7
  %lnbNJ = inttoptr i64 %lnbNI to i64*
  %lnbNK = load i64, i64* %lnbNJ, !tbaa !3
  store i64 %lnbNK, i64* %lsb2A
  %lnbNN = load i64, i64* %R1_Var
  %lnbNO = add i64 %lnbNN, 15
  %lnbNP = inttoptr i64 %lnbNO to i64*
  %lnbNQ = load i64, i64* %lnbNP, !tbaa !3
  store i64 %lnbNQ, i64* %lsb2B
  %lnbNS = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb2C_info$def to i64
  %lnbNR = load i64*, i64** %Hp_Var
  %lnbNT = getelementptr inbounds i64, i64* %lnbNR, i32 -6
  store i64 %lnbNS, i64* %lnbNT, !tbaa !2
  %lnbNV = load i64*, i64** %Sp_Var
  %lnbNW = getelementptr inbounds i64, i64* %lnbNV, i32 2
  %lnbNX = bitcast i64* %lnbNW to i64*
  %lnbNY = load i64, i64* %lnbNX, !tbaa !1
  %lnbNU = load i64*, i64** %Hp_Var
  %lnbNZ = getelementptr inbounds i64, i64* %lnbNU, i32 -4
  store i64 %lnbNY, i64* %lnbNZ, !tbaa !2
  %lnbO1 = load i64, i64* %lsb2A
  %lnbO0 = load i64*, i64** %Hp_Var
  %lnbO2 = getelementptr inbounds i64, i64* %lnbO0, i32 -3
  store i64 %lnbO1, i64* %lnbO2, !tbaa !2
  %lnbO4 = ptrtoint i8* @Disk_VS_con_info to i64
  %lnbO3 = load i64*, i64** %Hp_Var
  %lnbO5 = getelementptr inbounds i64, i64* %lnbO3, i32 -2
  store i64 %lnbO4, i64* %lnbO5, !tbaa !2
  %lnbO7 = load i64*, i64** %Hp_Var
  %lnbO8 = getelementptr inbounds i64, i64* %lnbO7, i32 -6
  %lnbO9 = ptrtoint i64* %lnbO8 to i64
  %lnbO6 = load i64*, i64** %Hp_Var
  %lnbOa = getelementptr inbounds i64, i64* %lnbO6, i32 -1
  store i64 %lnbO9, i64* %lnbOa, !tbaa !2
  %lnbOc = load i64, i64* %lsb2B
  %lnbOb = load i64*, i64** %Hp_Var
  %lnbOd = getelementptr inbounds i64, i64* %lnbOb, i32 0
  store i64 %lnbOc, i64* %lnbOd, !tbaa !2
  %lnbOe = load i64*, i64** %Sp_Var
  %lnbOf = getelementptr inbounds i64, i64* %lnbOe, i32 1
  %lnbOg = bitcast i64* %lnbOf to i64*
  %lnbOh = load i64, i64* %lnbOg, !tbaa !1
  store i64 %lnbOh, i64* %lsb2v
  %lnbOi = load i64, i64* %lsb2v
  %lnbOj = add i64 %lnbOi, 8
  %lnbOl = load i64*, i64** %Hp_Var
  %lnbOm = ptrtoint i64* %lnbOl to i64
  %lnbOn = add i64 %lnbOm, -15
  %lnbOo = inttoptr i64 %lnbOj to i64*
  store i64 %lnbOn, i64* %lnbOo, !tbaa !5
  %lnbOp = ptrtoint i64* %Base_Arg to i64
  %lnbOq = inttoptr i64 %lnbOp to i8*
  %lnbOr = load i64, i64* %lsb2v
  %lnbOs = inttoptr i64 %lnbOr to i8*
  %lnbOt = bitcast i8* @dirty_MUT_VAR to void (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  call ccc void (i8*, i8*) %lnbOt( i8* %lnbOq, i8* %lnbOs ) nounwind
  %lnbOu = ptrtoint i8* @ghczmprim_GHCziTuple_Z0T_closure to i64
  %lnbOv = add i64 %lnbOu, 1
  store i64 %lnbOv, i64* %R1_Var
  %lnbOw = load i64*, i64** %Sp_Var
  %lnbOx = getelementptr inbounds i64, i64* %lnbOw, i32 3
  %lnbOy = ptrtoint i64* %lnbOx to i64
  %lnbOz = inttoptr i64 %lnbOy to i64*
  store i64* %lnbOz, i64** %Sp_Var
  %lnbOA = load i64*, i64** %Sp_Var
  %lnbOB = getelementptr inbounds i64, i64* %lnbOA, i32 0
  %lnbOC = bitcast i64* %lnbOB to i64*
  %lnbOD = load i64, i64* %lnbOC, !tbaa !1
  %lnbOE = inttoptr i64 %lnbOD to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbOF = load i64*, i64** %Sp_Var
  %lnbOG = load i64*, i64** %Hp_Var
  %lnbOH = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbOE( i64* %Base_Arg, i64* %lnbOF, i64* %lnbOG, i64 %lnbOH, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbLC:
  %lnbOI = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 56, i64* %lnbOI, !tbaa !4
  %lnbOJ = load i64, i64* %R1_Var
  store i64 %lnbOJ, i64* %R1_Var
  %lnbOK = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbOL = load i64*, i64** %Sp_Var
  %lnbOM = load i64*, i64** %Hp_Var
  %lnbON = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbOK( i64* %Base_Arg, i64* %lnbOL, i64* %lnbOM, i64 %lnbON, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_varzudelete_closure_struct = type <{i64, i64}>
@Disk_varzudelete_closure$def = internal global %Disk_varzudelete_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzudelete_info$def to i64), i64 0}>
@Disk_varzudelete_closure = alias i8* bitcast (%Disk_varzudelete_closure_struct* @Disk_varzudelete_closure$def to i8*)
@Disk_varzudelete_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzudelete_info$def to i8*)
define ghccc void @Disk_varzudelete_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzudelete_info$def to i64)),i64 64), i64 12884901903, i64 0, i64 4294967311}>
{
cbOT:
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnbOW = load i64, i64* %R3_Var
  store i64 %lnbOW, i64* %R3_Var
  %lnbOX = load i64, i64* %R2_Var
  store i64 %lnbOX, i64* %R2_Var
  %lnbOY = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzudelete1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbOZ = load i64, i64* %R2_Var
  %lnbP0 = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbOY( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnbOZ, i64 %lnbP0, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_varzuset1_closure_struct = type <{i64, i64}>
@Disk_varzuset1_closure$def = internal global %Disk_varzuset1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzuset1_info$def to i64), i64 0}>
@Disk_varzuset1_closure = alias i8* bitcast (%Disk_varzuset1_closure_struct* @Disk_varzuset1_closure$def to i8*)
@sb2U_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb2U_info$def to i8*)
define internal ghccc void @sb2U_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb2U_info$def to i64)),i64 32), i64 3, i64 4294967312}>
{
cbPw:
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbPM = load i64*, i64** %Sp_Var
  %lnbPN = getelementptr inbounds i64, i64* %lnbPM, i32 -2
  %lnbPO = ptrtoint i64* %lnbPN to i64
  %lnbPP = icmp ult i64 %lnbPO, %SpLim_Arg
  br i1 %lnbPP, label %cbPx, label %cbPy
cbPy:
  %lnbPR = ptrtoint i8* @stg_upd_frame_info to i64
  %lnbPQ = load i64*, i64** %Sp_Var
  %lnbPS = getelementptr inbounds i64, i64* %lnbPQ, i32 -2
  store i64 %lnbPR, i64* %lnbPS, !tbaa !1
  %lnbPU = load i64, i64* %R1_Var
  %lnbPT = load i64*, i64** %Sp_Var
  %lnbPV = getelementptr inbounds i64, i64* %lnbPT, i32 -1
  store i64 %lnbPU, i64* %lnbPV, !tbaa !1
  %lnbPY = load i64, i64* %R1_Var
  %lnbPZ = add i64 %lnbPY, 32
  %lnbQ0 = inttoptr i64 %lnbPZ to i64*
  %lnbQ1 = load i64, i64* %lnbQ0, !tbaa !3
  store i64 %lnbQ1, i64* %R4_Var
  %lnbQ4 = load i64, i64* %R1_Var
  %lnbQ5 = add i64 %lnbQ4, 24
  %lnbQ6 = inttoptr i64 %lnbQ5 to i64*
  %lnbQ7 = load i64, i64* %lnbQ6, !tbaa !3
  store i64 %lnbQ7, i64* %R3_Var
  %lnbQa = load i64, i64* %R1_Var
  %lnbQb = add i64 %lnbQa, 16
  %lnbQc = inttoptr i64 %lnbQb to i64*
  %lnbQd = load i64, i64* %lnbQc, !tbaa !3
  store i64 %lnbQd, i64* %R2_Var
  %lnbQe = load i64*, i64** %Sp_Var
  %lnbQf = getelementptr inbounds i64, i64* %lnbQe, i32 -2
  %lnbQg = ptrtoint i64* %lnbQf to i64
  %lnbQh = inttoptr i64 %lnbQg to i64*
  store i64* %lnbQh, i64** %Sp_Var
  %lnbQi = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdsinsertzuzdsgo4_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbQj = load i64*, i64** %Sp_Var
  %lnbQk = load i64, i64* %R1_Var
  %lnbQl = load i64, i64* %R2_Var
  %lnbQm = load i64, i64* %R3_Var
  %lnbQn = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbQi( i64* %Base_Arg, i64* %lnbQj, i64* %Hp_Arg, i64 %lnbQk, i64 %lnbQl, i64 %lnbQm, i64 %lnbQn, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbPx:
  %lnbQo = load i64, i64* %R1_Var
  store i64 %lnbQo, i64* %R1_Var
  %lnbQp = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lnbQq = bitcast i64* %lnbQp to i64*
  %lnbQr = load i64, i64* %lnbQq, !tbaa !4
  %lnbQs = inttoptr i64 %lnbQr to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbQt = load i64*, i64** %Sp_Var
  %lnbQu = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbQs( i64* %Base_Arg, i64* %lnbQt, i64* %Hp_Arg, i64 %lnbQu, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@Disk_varzuset1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzuset1_info$def to i8*)
define ghccc void @Disk_varzuset1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzuset1_info$def to i64)),i64 32), i64 17179869207, i64 0, i64 141733920783}>
{
cbPz:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R4_Var = alloca i64, i32 1
  store i64 %R4_Arg, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnbQv = load i64*, i64** %Sp_Var
  %lnbQw = getelementptr inbounds i64, i64* %lnbQv, i32 -4
  %lnbQx = ptrtoint i64* %lnbQw to i64
  %lnbQy = icmp ult i64 %lnbQx, %SpLim_Arg
  br i1 %lnbQy, label %cbPA, label %cbPB
cbPB:
  %lnbQA = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbPi_info$def to i64
  %lnbQz = load i64*, i64** %Sp_Var
  %lnbQB = getelementptr inbounds i64, i64* %lnbQz, i32 -3
  store i64 %lnbQA, i64* %lnbQB, !tbaa !1
  %lnbQC = load i64, i64* %R2_Var
  store i64 %lnbQC, i64* %R1_Var
  %lnbQE = load i64, i64* %R3_Var
  %lnbQD = load i64*, i64** %Sp_Var
  %lnbQF = getelementptr inbounds i64, i64* %lnbQD, i32 -2
  store i64 %lnbQE, i64* %lnbQF, !tbaa !1
  %lnbQH = load i64, i64* %R4_Var
  %lnbQG = load i64*, i64** %Sp_Var
  %lnbQI = getelementptr inbounds i64, i64* %lnbQG, i32 -1
  store i64 %lnbQH, i64* %lnbQI, !tbaa !1
  %lnbQJ = load i64*, i64** %Sp_Var
  %lnbQK = getelementptr inbounds i64, i64* %lnbQJ, i32 -3
  %lnbQL = ptrtoint i64* %lnbQK to i64
  %lnbQM = inttoptr i64 %lnbQL to i64*
  store i64* %lnbQM, i64** %Sp_Var
  %lnbQN = load i64, i64* %R1_Var
  %lnbQO = and i64 %lnbQN, 7
  %lnbQP = icmp ne i64 %lnbQO, 0
  br i1 %lnbQP, label %ubPL, label %cbPj
cbPj:
  %lnbQR = load i64, i64* %R1_Var
  %lnbQS = inttoptr i64 %lnbQR to i64*
  %lnbQT = load i64, i64* %lnbQS, !tbaa !3
  %lnbQU = inttoptr i64 %lnbQT to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbQV = load i64*, i64** %Sp_Var
  %lnbQW = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbQU( i64* %Base_Arg, i64* %lnbQV, i64* %Hp_Arg, i64 %lnbQW, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubPL:
  %lnbQX = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbPi_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbQY = load i64*, i64** %Sp_Var
  %lnbQZ = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbQX( i64* %Base_Arg, i64* %lnbQY, i64* %Hp_Arg, i64 %lnbQZ, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbPA:
  %lnbR0 = load i64, i64* %R4_Var
  store i64 %lnbR0, i64* %R4_Var
  %lnbR1 = load i64, i64* %R3_Var
  store i64 %lnbR1, i64* %R3_Var
  %lnbR2 = load i64, i64* %R2_Var
  store i64 %lnbR2, i64* %R2_Var
  %lnbR3 = ptrtoint %Disk_varzuset1_closure_struct* @Disk_varzuset1_closure$def to i64
  store i64 %lnbR3, i64* %R1_Var
  %lnbR4 = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lnbR5 = bitcast i64* %lnbR4 to i64*
  %lnbR6 = load i64, i64* %lnbR5, !tbaa !4
  %lnbR7 = inttoptr i64 %lnbR6 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbR8 = load i64*, i64** %Sp_Var
  %lnbR9 = load i64, i64* %R1_Var
  %lnbRa = load i64, i64* %R2_Var
  %lnbRb = load i64, i64* %R3_Var
  %lnbRc = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbR7( i64* %Base_Arg, i64* %lnbR8, i64* %Hp_Arg, i64 %lnbR9, i64 %lnbRa, i64 %lnbRb, i64 %lnbRc, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbPi_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbPi_info$def to i8*)
define internal ghccc void @cbPi_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbPi_info$def to i64)),i64 32), i64 2, i64 4294967328}>
{
cbPi:
  %lsb2N = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnbRe = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbPn_info$def to i64
  %lnbRd = load i64*, i64** %Sp_Var
  %lnbRf = getelementptr inbounds i64, i64* %lnbRd, i32 -1
  store i64 %lnbRe, i64* %lnbRf, !tbaa !1
  %lnbRi = load i64, i64* %R1_Var
  %lnbRj = add i64 %lnbRi, 23
  %lnbRk = inttoptr i64 %lnbRj to i64*
  %lnbRl = load i64, i64* %lnbRk, !tbaa !3
  store i64 %lnbRl, i64* %lsb2N
  %lnbRm = load i64, i64* %lsb2N
  %lnbRn = add i64 %lnbRm, 8
  %lnbRo = inttoptr i64 %lnbRn to i64*
  %lnbRp = load i64, i64* %lnbRo, !tbaa !5
  store i64 %lnbRp, i64* %R1_Var
  %lnbRr = load i64, i64* %lsb2N
  %lnbRq = load i64*, i64** %Sp_Var
  %lnbRs = getelementptr inbounds i64, i64* %lnbRq, i32 0
  store i64 %lnbRr, i64* %lnbRs, !tbaa !1
  %lnbRt = load i64*, i64** %Sp_Var
  %lnbRu = getelementptr inbounds i64, i64* %lnbRt, i32 -1
  %lnbRv = ptrtoint i64* %lnbRu to i64
  %lnbRw = inttoptr i64 %lnbRv to i64*
  store i64* %lnbRw, i64** %Sp_Var
  %lnbRx = load i64, i64* %R1_Var
  %lnbRy = and i64 %lnbRx, 7
  %lnbRz = icmp ne i64 %lnbRy, 0
  br i1 %lnbRz, label %ubPK, label %cbPo
cbPo:
  %lnbRB = load i64, i64* %R1_Var
  %lnbRC = inttoptr i64 %lnbRB to i64*
  %lnbRD = load i64, i64* %lnbRC, !tbaa !3
  %lnbRE = inttoptr i64 %lnbRD to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbRF = load i64*, i64** %Sp_Var
  %lnbRG = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbRE( i64* %Base_Arg, i64* %lnbRF, i64* %Hp_Arg, i64 %lnbRG, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubPK:
  %lnbRH = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbPn_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbRI = load i64*, i64** %Sp_Var
  %lnbRJ = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbRH( i64* %Base_Arg, i64* %lnbRI, i64* %Hp_Arg, i64 %lnbRJ, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbPn_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbPn_info$def to i8*)
define internal ghccc void @cbPn_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbPn_info$def to i64)),i64 32), i64 3, i64 4294967328}>
{
cbPn:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb2S = alloca i64, i32 1
  %lsb2T = alloca i64, i32 1
  %lsb2N = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnbRK = load i64*, i64** %Hp_Var
  %lnbRL = getelementptr inbounds i64, i64* %lnbRK, i32 8
  %lnbRM = ptrtoint i64* %lnbRL to i64
  %lnbRN = inttoptr i64 %lnbRM to i64*
  store i64* %lnbRN, i64** %Hp_Var
  %lnbRO = load i64*, i64** %Hp_Var
  %lnbRP = ptrtoint i64* %lnbRO to i64
  %lnbRQ = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnbRR = bitcast i64* %lnbRQ to i64*
  %lnbRS = load i64, i64* %lnbRR, !tbaa !4
  %lnbRT = icmp ugt i64 %lnbRP, %lnbRS
  br i1 %lnbRT, label %cbPF, label %cbPE
cbPE:
  %lnbRW = load i64, i64* %R1_Var
  %lnbRX = add i64 %lnbRW, 7
  %lnbRY = inttoptr i64 %lnbRX to i64*
  %lnbRZ = load i64, i64* %lnbRY, !tbaa !3
  store i64 %lnbRZ, i64* %lsb2S
  %lnbS2 = load i64, i64* %R1_Var
  %lnbS3 = add i64 %lnbS2, 15
  %lnbS4 = inttoptr i64 %lnbS3 to i64*
  %lnbS5 = load i64, i64* %lnbS4, !tbaa !3
  store i64 %lnbS5, i64* %lsb2T
  %lnbS7 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb2U_info$def to i64
  %lnbS6 = load i64*, i64** %Hp_Var
  %lnbS8 = getelementptr inbounds i64, i64* %lnbS6, i32 -7
  store i64 %lnbS7, i64* %lnbS8, !tbaa !2
  %lnbSa = load i64*, i64** %Sp_Var
  %lnbSb = getelementptr inbounds i64, i64* %lnbSa, i32 2
  %lnbSc = bitcast i64* %lnbSb to i64*
  %lnbSd = load i64, i64* %lnbSc, !tbaa !1
  %lnbS9 = load i64*, i64** %Hp_Var
  %lnbSe = getelementptr inbounds i64, i64* %lnbS9, i32 -5
  store i64 %lnbSd, i64* %lnbSe, !tbaa !2
  %lnbSg = load i64*, i64** %Sp_Var
  %lnbSh = getelementptr inbounds i64, i64* %lnbSg, i32 3
  %lnbSi = bitcast i64* %lnbSh to i64*
  %lnbSj = load i64, i64* %lnbSi, !tbaa !1
  %lnbSf = load i64*, i64** %Hp_Var
  %lnbSk = getelementptr inbounds i64, i64* %lnbSf, i32 -4
  store i64 %lnbSj, i64* %lnbSk, !tbaa !2
  %lnbSm = load i64, i64* %lsb2S
  %lnbSl = load i64*, i64** %Hp_Var
  %lnbSn = getelementptr inbounds i64, i64* %lnbSl, i32 -3
  store i64 %lnbSm, i64* %lnbSn, !tbaa !2
  %lnbSp = ptrtoint i8* @Disk_VS_con_info to i64
  %lnbSo = load i64*, i64** %Hp_Var
  %lnbSq = getelementptr inbounds i64, i64* %lnbSo, i32 -2
  store i64 %lnbSp, i64* %lnbSq, !tbaa !2
  %lnbSs = load i64*, i64** %Hp_Var
  %lnbSt = getelementptr inbounds i64, i64* %lnbSs, i32 -7
  %lnbSu = ptrtoint i64* %lnbSt to i64
  %lnbSr = load i64*, i64** %Hp_Var
  %lnbSv = getelementptr inbounds i64, i64* %lnbSr, i32 -1
  store i64 %lnbSu, i64* %lnbSv, !tbaa !2
  %lnbSx = load i64, i64* %lsb2T
  %lnbSw = load i64*, i64** %Hp_Var
  %lnbSy = getelementptr inbounds i64, i64* %lnbSw, i32 0
  store i64 %lnbSx, i64* %lnbSy, !tbaa !2
  %lnbSz = load i64*, i64** %Sp_Var
  %lnbSA = getelementptr inbounds i64, i64* %lnbSz, i32 1
  %lnbSB = bitcast i64* %lnbSA to i64*
  %lnbSC = load i64, i64* %lnbSB, !tbaa !1
  store i64 %lnbSC, i64* %lsb2N
  %lnbSD = load i64, i64* %lsb2N
  %lnbSE = add i64 %lnbSD, 8
  %lnbSG = load i64*, i64** %Hp_Var
  %lnbSH = ptrtoint i64* %lnbSG to i64
  %lnbSI = add i64 %lnbSH, -15
  %lnbSJ = inttoptr i64 %lnbSE to i64*
  store i64 %lnbSI, i64* %lnbSJ, !tbaa !5
  %lnbSK = ptrtoint i64* %Base_Arg to i64
  %lnbSL = inttoptr i64 %lnbSK to i8*
  %lnbSM = load i64, i64* %lsb2N
  %lnbSN = inttoptr i64 %lnbSM to i8*
  %lnbSO = bitcast i8* @dirty_MUT_VAR to void (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  call ccc void (i8*, i8*) %lnbSO( i8* %lnbSL, i8* %lnbSN ) nounwind
  %lnbSP = ptrtoint i8* @ghczmprim_GHCziTuple_Z0T_closure to i64
  %lnbSQ = add i64 %lnbSP, 1
  store i64 %lnbSQ, i64* %R1_Var
  %lnbSR = load i64*, i64** %Sp_Var
  %lnbSS = getelementptr inbounds i64, i64* %lnbSR, i32 4
  %lnbST = ptrtoint i64* %lnbSS to i64
  %lnbSU = inttoptr i64 %lnbST to i64*
  store i64* %lnbSU, i64** %Sp_Var
  %lnbSV = load i64*, i64** %Sp_Var
  %lnbSW = getelementptr inbounds i64, i64* %lnbSV, i32 0
  %lnbSX = bitcast i64* %lnbSW to i64*
  %lnbSY = load i64, i64* %lnbSX, !tbaa !1
  %lnbSZ = inttoptr i64 %lnbSY to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbT0 = load i64*, i64** %Sp_Var
  %lnbT1 = load i64*, i64** %Hp_Var
  %lnbT2 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbSZ( i64* %Base_Arg, i64* %lnbT0, i64* %lnbT1, i64 %lnbT2, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbPF:
  %lnbT3 = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 64, i64* %lnbT3, !tbaa !4
  %lnbT4 = load i64, i64* %R1_Var
  store i64 %lnbT4, i64* %R1_Var
  %lnbT5 = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbT6 = load i64*, i64** %Sp_Var
  %lnbT7 = load i64*, i64** %Hp_Var
  %lnbT8 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbT5( i64* %Base_Arg, i64* %lnbT6, i64* %lnbT7, i64 %lnbT8, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_varzuset_closure_struct = type <{i64, i64}>
@Disk_varzuset_closure$def = internal global %Disk_varzuset_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzuset_info$def to i64), i64 0}>
@Disk_varzuset_closure = alias i8* bitcast (%Disk_varzuset_closure_struct* @Disk_varzuset_closure$def to i8*)
@Disk_varzuset_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzuset_info$def to i8*)
define ghccc void @Disk_varzuset_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzuset_info$def to i64)),i64 72), i64 17179869207, i64 0, i64 4294967311}>
{
cbTe:
  %R4_Var = alloca i64, i32 1
  store i64 %R4_Arg, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnbTh = load i64, i64* %R4_Var
  store i64 %lnbTh, i64* %R4_Var
  %lnbTi = load i64, i64* %R3_Var
  store i64 %lnbTi, i64* %R3_Var
  %lnbTj = load i64, i64* %R2_Var
  store i64 %lnbTj, i64* %R2_Var
  %lnbTk = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzuset1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbTl = load i64, i64* %R2_Var
  %lnbTm = load i64, i64* %R3_Var
  %lnbTn = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbTk( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnbTl, i64 %lnbTm, i64 %lnbTn, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%rb0F_closure_struct = type <{i64, i64}>
@rb0F_closure$def = internal global %rb0F_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_Izh_static_info to i64), i64 201}>
@rb0F_closure = internal alias i8* bitcast (%rb0F_closure_struct* @rb0F_closure$def to i8*)
%rb0G_closure_struct = type <{i64, i64}>
@rb0G_closure$def = internal global %rb0G_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_Izh_static_info to i64), i64 16}>
@rb0G_closure = internal alias i8* bitcast (%rb0G_closure_struct* @rb0G_closure$def to i8*)
%rb0H_closure_struct = type <{i64, i64}>
@rb0H_closure$def = internal global %rb0H_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_Izh_static_info to i64), i64 49}>
@rb0H_closure = internal alias i8* bitcast (%rb0H_closure_struct* @rb0H_closure$def to i8*)
%rb0I_closure_struct = type <{i64, i64, i64, i64, i64, i64, i64, i64, i64}>
@rb0I_closure$def = internal global %rb0I_closure_struct<{i64 ptrtoint (i8* @base_GHCziStackziTypes_SrcLoc_static_info to i64), i64 ptrtoint (%rb0s_closure_struct* @rb0s_closure$def to i64), i64 ptrtoint (%rb0t_closure_struct* @rb0t_closure$def to i64), i64 ptrtoint (%rb0u_closure_struct* @rb0u_closure$def to i64), i64 add (i64 ptrtoint (%rb0F_closure_struct* @rb0F_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%rb0G_closure_struct* @rb0G_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%rb0F_closure_struct* @rb0F_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%rb0H_closure_struct* @rb0H_closure$def to i64),i64 1), i64 0}>
@rb0I_closure = internal alias i8* bitcast (%rb0I_closure_struct* @rb0I_closure$def to i8*)
%rb0J_closure_struct = type <{i64, i64, i64, i64, i64}>
@rb0J_closure$def = internal global %rb0J_closure_struct<{i64 ptrtoint (i8* @base_GHCziStackziTypes_PushCallStack_static_info to i64), i64 ptrtoint (%rb0y_closure_struct* @rb0y_closure$def to i64), i64 add (i64 ptrtoint (%rb0I_closure_struct* @rb0I_closure$def to i64),i64 1), i64 add (i64 ptrtoint (i8* @base_GHCziStackziTypes_EmptyCallStack_closure to i64),i64 1), i64 0}>
@rb0J_closure = internal alias i8* bitcast (%rb0J_closure_struct* @rb0J_closure$def to i8*)
%rb0K_closure_struct = type <{i64, i64, i64, i64}>
%cbTA_str_struct = type <{[26 x i8]}>
@rb0K_closure$def = internal global %rb0K_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rb0K_info$def to i64), i64 0, i64 0, i64 0}>
@rb0K_closure = internal alias i8* bitcast (%rb0K_closure_struct* @rb0K_closure$def to i8*)
@cbTA_str$def = internal constant %cbTA_str_struct<{[26 x i8] [i8 118, i8 97, i8 114, i8 95, i8 103, i8 101, i8 116, i8 32, i8 111, i8 102, i8 32, i8 117, i8 110, i8 115, i8 101, i8 116, i8 32, i8 118, i8 97, i8 114, i8 105, i8 97, i8 98, i8 108, i8 101, i8 0]}>
@cbTA_str = internal alias i8* bitcast (%cbTA_str_struct* @cbTA_str$def to i8*)
@rb0K_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rb0K_info$def to i8*)
define internal ghccc void @rb0K_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 22}>
{
cbTB:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %lcbTx = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbTE = load i64*, i64** %Sp_Var
  %lnbTF = getelementptr inbounds i64, i64* %lnbTE, i32 -2
  %lnbTG = ptrtoint i64* %lnbTF to i64
  %lnbTH = icmp ult i64 %lnbTG, %SpLim_Arg
  br i1 %lnbTH, label %cbTC, label %cbTD
cbTD:
  %lnbTI = ptrtoint i64* %Base_Arg to i64
  %lnbTJ = inttoptr i64 %lnbTI to i8*
  %lnbTK = load i64, i64* %R1_Var
  %lnbTL = inttoptr i64 %lnbTK to i8*
  %lnbTM = bitcast i8* @newCAF to i8* (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lnbTN = call ccc i8* (i8*, i8*) %lnbTM( i8* %lnbTJ, i8* %lnbTL ) nounwind
  %lnbTO = ptrtoint i8* %lnbTN to i64
  store i64 %lnbTO, i64* %lcbTx
  %lnbTP = load i64, i64* %lcbTx
  %lnbTQ = icmp eq i64 %lnbTP, 0
  br i1 %lnbTQ, label %cbTz, label %cbTy
cbTy:
  %lnbTS = ptrtoint i8* @stg_bh_upd_frame_info to i64
  %lnbTR = load i64*, i64** %Sp_Var
  %lnbTT = getelementptr inbounds i64, i64* %lnbTR, i32 -2
  store i64 %lnbTS, i64* %lnbTT, !tbaa !1
  %lnbTV = load i64, i64* %lcbTx
  %lnbTU = load i64*, i64** %Sp_Var
  %lnbTW = getelementptr inbounds i64, i64* %lnbTU, i32 -1
  store i64 %lnbTV, i64* %lnbTW, !tbaa !1
  %lnbTX = ptrtoint %cbTA_str_struct* @cbTA_str$def to i64
  store i64 %lnbTX, i64* %R2_Var
  %lnbTY = load i64*, i64** %Sp_Var
  %lnbTZ = getelementptr inbounds i64, i64* %lnbTY, i32 -2
  %lnbU0 = ptrtoint i64* %lnbTZ to i64
  %lnbU1 = inttoptr i64 %lnbU0 to i64*
  store i64* %lnbU1, i64** %Sp_Var
  %lnbU2 = bitcast i8* @ghczmprim_GHCziCString_unpackCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbU3 = load i64*, i64** %Sp_Var
  %lnbU4 = load i64, i64* %R1_Var
  %lnbU5 = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbU2( i64* %Base_Arg, i64* %lnbU3, i64* %Hp_Arg, i64 %lnbU4, i64 %lnbU5, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbTz:
  %lnbU7 = load i64, i64* %R1_Var
  %lnbU8 = inttoptr i64 %lnbU7 to i64*
  %lnbU9 = load i64, i64* %lnbU8, !tbaa !3
  %lnbUa = inttoptr i64 %lnbU9 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbUb = load i64*, i64** %Sp_Var
  %lnbUc = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbUa( i64* %Base_Arg, i64* %lnbUb, i64* %Hp_Arg, i64 %lnbUc, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbTC:
  %lnbUd = load i64, i64* %R1_Var
  store i64 %lnbUd, i64* %R1_Var
  %lnbUe = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lnbUf = bitcast i64* %lnbUe to i64*
  %lnbUg = load i64, i64* %lnbUf, !tbaa !4
  %lnbUh = inttoptr i64 %lnbUg to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbUi = load i64*, i64** %Sp_Var
  %lnbUj = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbUh( i64* %Base_Arg, i64* %lnbUi, i64* %Hp_Arg, i64 %lnbUj, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_varzuget2_closure_struct = type <{i64, i64, i64, i64}>
@Disk_varzuget2_closure$def = internal global %Disk_varzuget2_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzuget2_info$def to i64), i64 0, i64 0, i64 0}>
@Disk_varzuget2_closure = alias i8* bitcast (%Disk_varzuget2_closure_struct* @Disk_varzuget2_closure$def to i8*)
@Disk_varzuget2_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzuget2_info$def to i8*)
define ghccc void @Disk_varzuget2_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzuget2_info$def to i64)),i64 80), i64 0, i64 30064771094}>
{
cbUr:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %lcbUo = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbUu = load i64*, i64** %Sp_Var
  %lnbUv = getelementptr inbounds i64, i64* %lnbUu, i32 -2
  %lnbUw = ptrtoint i64* %lnbUv to i64
  %lnbUx = icmp ult i64 %lnbUw, %SpLim_Arg
  br i1 %lnbUx, label %cbUs, label %cbUt
cbUt:
  %lnbUy = ptrtoint i64* %Base_Arg to i64
  %lnbUz = inttoptr i64 %lnbUy to i8*
  %lnbUA = load i64, i64* %R1_Var
  %lnbUB = inttoptr i64 %lnbUA to i8*
  %lnbUC = bitcast i8* @newCAF to i8* (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lnbUD = call ccc i8* (i8*, i8*) %lnbUC( i8* %lnbUz, i8* %lnbUB ) nounwind
  %lnbUE = ptrtoint i8* %lnbUD to i64
  store i64 %lnbUE, i64* %lcbUo
  %lnbUF = load i64, i64* %lcbUo
  %lnbUG = icmp eq i64 %lnbUF, 0
  br i1 %lnbUG, label %cbUq, label %cbUp
cbUp:
  %lnbUI = ptrtoint i8* @stg_bh_upd_frame_info to i64
  %lnbUH = load i64*, i64** %Sp_Var
  %lnbUJ = getelementptr inbounds i64, i64* %lnbUH, i32 -2
  store i64 %lnbUI, i64* %lnbUJ, !tbaa !1
  %lnbUL = load i64, i64* %lcbUo
  %lnbUK = load i64*, i64** %Sp_Var
  %lnbUM = getelementptr inbounds i64, i64* %lnbUK, i32 -1
  store i64 %lnbUL, i64* %lnbUM, !tbaa !1
  %lnbUN = ptrtoint %rb0K_closure_struct* @rb0K_closure$def to i64
  store i64 %lnbUN, i64* %R3_Var
  %lnbUO = ptrtoint %rb0J_closure_struct* @rb0J_closure$def to i64
  %lnbUP = add i64 %lnbUO, 2
  store i64 %lnbUP, i64* %R2_Var
  %lnbUQ = load i64*, i64** %Sp_Var
  %lnbUR = getelementptr inbounds i64, i64* %lnbUQ, i32 -2
  %lnbUS = ptrtoint i64* %lnbUR to i64
  %lnbUT = inttoptr i64 %lnbUS to i64*
  store i64* %lnbUT, i64** %Sp_Var
  %lnbUU = bitcast i8* @base_GHCziErr_error_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbUV = load i64*, i64** %Sp_Var
  %lnbUW = load i64, i64* %R1_Var
  %lnbUX = load i64, i64* %R2_Var
  %lnbUY = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbUU( i64* %Base_Arg, i64* %lnbUV, i64* %Hp_Arg, i64 %lnbUW, i64 %lnbUX, i64 %lnbUY, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbUq:
  %lnbV0 = load i64, i64* %R1_Var
  %lnbV1 = inttoptr i64 %lnbV0 to i64*
  %lnbV2 = load i64, i64* %lnbV1, !tbaa !3
  %lnbV3 = inttoptr i64 %lnbV2 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbV4 = load i64*, i64** %Sp_Var
  %lnbV5 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbV3( i64* %Base_Arg, i64* %lnbV4, i64* %Hp_Arg, i64 %lnbV5, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbUs:
  %lnbV6 = load i64, i64* %R1_Var
  store i64 %lnbV6, i64* %R1_Var
  %lnbV7 = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lnbV8 = bitcast i64* %lnbV7 to i64*
  %lnbV9 = load i64, i64* %lnbV8, !tbaa !4
  %lnbVa = inttoptr i64 %lnbV9 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbVb = load i64*, i64** %Sp_Var
  %lnbVc = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbVa( i64* %Base_Arg, i64* %lnbVb, i64* %Hp_Arg, i64 %lnbVc, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_varzuget1_closure_struct = type <{i64, i64}>
@Disk_varzuget1_closure$def = internal global %Disk_varzuget1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzuget1_info$def to i64), i64 0}>
@Disk_varzuget1_closure = alias i8* bitcast (%Disk_varzuget1_closure_struct* @Disk_varzuget1_closure$def to i8*)
@Disk_varzuget1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzuget1_info$def to i8*)
define ghccc void @Disk_varzuget1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzuget1_info$def to i64)),i64 104), i64 12884901903, i64 0, i64 12884901903}>
{
cbVw:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnbVW = load i64*, i64** %Sp_Var
  %lnbVX = getelementptr inbounds i64, i64* %lnbVW, i32 -2
  %lnbVY = ptrtoint i64* %lnbVX to i64
  %lnbVZ = icmp ult i64 %lnbVY, %SpLim_Arg
  br i1 %lnbVZ, label %cbVA, label %cbVB
cbVB:
  %lnbW1 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbVt_info$def to i64
  %lnbW0 = load i64*, i64** %Sp_Var
  %lnbW2 = getelementptr inbounds i64, i64* %lnbW0, i32 -2
  store i64 %lnbW1, i64* %lnbW2, !tbaa !1
  %lnbW3 = load i64, i64* %R2_Var
  store i64 %lnbW3, i64* %R1_Var
  %lnbW5 = load i64, i64* %R3_Var
  %lnbW4 = load i64*, i64** %Sp_Var
  %lnbW6 = getelementptr inbounds i64, i64* %lnbW4, i32 -1
  store i64 %lnbW5, i64* %lnbW6, !tbaa !1
  %lnbW7 = load i64*, i64** %Sp_Var
  %lnbW8 = getelementptr inbounds i64, i64* %lnbW7, i32 -2
  %lnbW9 = ptrtoint i64* %lnbW8 to i64
  %lnbWa = inttoptr i64 %lnbW9 to i64*
  store i64* %lnbWa, i64** %Sp_Var
  %lnbWb = load i64, i64* %R1_Var
  %lnbWc = and i64 %lnbWb, 7
  %lnbWd = icmp ne i64 %lnbWc, 0
  br i1 %lnbWd, label %ubVV, label %cbVu
cbVu:
  %lnbWf = load i64, i64* %R1_Var
  %lnbWg = inttoptr i64 %lnbWf to i64*
  %lnbWh = load i64, i64* %lnbWg, !tbaa !3
  %lnbWi = inttoptr i64 %lnbWh to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbWj = load i64*, i64** %Sp_Var
  %lnbWk = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbWi( i64* %Base_Arg, i64* %lnbWj, i64* %Hp_Arg, i64 %lnbWk, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubVV:
  %lnbWl = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbVt_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbWm = load i64*, i64** %Sp_Var
  %lnbWn = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbWl( i64* %Base_Arg, i64* %lnbWm, i64* %Hp_Arg, i64 %lnbWn, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbVA:
  %lnbWo = load i64, i64* %R3_Var
  store i64 %lnbWo, i64* %R3_Var
  %lnbWp = load i64, i64* %R2_Var
  store i64 %lnbWp, i64* %R2_Var
  %lnbWq = ptrtoint %Disk_varzuget1_closure_struct* @Disk_varzuget1_closure$def to i64
  store i64 %lnbWq, i64* %R1_Var
  %lnbWr = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lnbWs = bitcast i64* %lnbWr to i64*
  %lnbWt = load i64, i64* %lnbWs, !tbaa !4
  %lnbWu = inttoptr i64 %lnbWt to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbWv = load i64*, i64** %Sp_Var
  %lnbWw = load i64, i64* %R1_Var
  %lnbWx = load i64, i64* %R2_Var
  %lnbWy = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbWu( i64* %Base_Arg, i64* %lnbWv, i64* %Hp_Arg, i64 %lnbWw, i64 %lnbWx, i64 %lnbWy, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbVt_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbVt_info$def to i8*)
define internal ghccc void @cbVt_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbVt_info$def to i64)),i64 104), i64 1, i64 4294967328}>
{
cbVt:
  %lsb37 = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbWB = load i64, i64* %R1_Var
  %lnbWC = add i64 %lnbWB, 23
  %lnbWD = inttoptr i64 %lnbWC to i64*
  %lnbWE = load i64, i64* %lnbWD, !tbaa !3
  %lnbWF = add i64 %lnbWE, 8
  %lnbWG = inttoptr i64 %lnbWF to i64*
  %lnbWH = load i64, i64* %lnbWG, !tbaa !5
  store i64 %lnbWH, i64* %lsb37
  %lnbWI = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbVz_info$def to i64
  %lnbWJ = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lnbWI, i64* %lnbWJ, !tbaa !1
  %lnbWK = load i64, i64* %lsb37
  store i64 %lnbWK, i64* %R1_Var
  %lnbWL = load i64, i64* %R1_Var
  %lnbWM = and i64 %lnbWL, 7
  %lnbWN = icmp ne i64 %lnbWM, 0
  br i1 %lnbWN, label %ubVU, label %cbVD
cbVD:
  %lnbWP = load i64, i64* %R1_Var
  %lnbWQ = inttoptr i64 %lnbWP to i64*
  %lnbWR = load i64, i64* %lnbWQ, !tbaa !3
  %lnbWS = inttoptr i64 %lnbWR to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbWT = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbWS( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbWT, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ubVU:
  %lnbWU = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbVz_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbWV = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbWU( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnbWV, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbVz_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbVz_info$def to i8*)
define internal ghccc void @cbVz_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbVz_info$def to i64)),i64 104), i64 1, i64 4294967328}>
{
cbVz:
  %lsb2Y = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnbWW = load i64*, i64** %Sp_Var
  %lnbWX = getelementptr inbounds i64, i64* %lnbWW, i32 1
  %lnbWY = bitcast i64* %lnbWX to i64*
  %lnbWZ = load i64, i64* %lnbWY, !tbaa !1
  store i64 %lnbWZ, i64* %lsb2Y
  %lnbX1 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbVH_info$def to i64
  %lnbX0 = load i64*, i64** %Sp_Var
  %lnbX2 = getelementptr inbounds i64, i64* %lnbX0, i32 1
  store i64 %lnbX1, i64* %lnbX2, !tbaa !1
  %lnbX3 = add i64 %R1_Arg, 7
  %lnbX4 = inttoptr i64 %lnbX3 to i64*
  %lnbX5 = load i64, i64* %lnbX4, !tbaa !3
  store i64 %lnbX5, i64* %R3_Var
  %lnbX6 = load i64, i64* %lsb2Y
  store i64 %lnbX6, i64* %R2_Var
  %lnbX7 = load i64*, i64** %Sp_Var
  %lnbX8 = getelementptr inbounds i64, i64* %lnbX7, i32 1
  %lnbX9 = ptrtoint i64* %lnbX8 to i64
  %lnbXa = inttoptr i64 %lnbX9 to i64*
  store i64* %lnbXa, i64** %Sp_Var
  %lnbXb = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdslookup1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbXc = load i64*, i64** %Sp_Var
  %lnbXd = load i64, i64* %R2_Var
  %lnbXe = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbXb( i64* %Base_Arg, i64* %lnbXc, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnbXd, i64 %lnbXe, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbVH_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbVH_info$def to i8*)
define internal ghccc void @cbVH_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbVH_info$def to i64)),i64 104), i64 0, i64 4294967328}>
{
cbVH:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnbXf = load i64, i64* %R1_Var
  %lnbXg = and i64 %lnbXf, 7
  switch i64 %lnbXg, label %cbVO [i64 1, label %cbVO
i64 2, label %cbVS]
cbVO:
  %lnbXh = ptrtoint %Disk_varzuget2_closure_struct* @Disk_varzuget2_closure$def to i64
  store i64 %lnbXh, i64* %R1_Var
  %lnbXi = load i64*, i64** %Sp_Var
  %lnbXj = getelementptr inbounds i64, i64* %lnbXi, i32 1
  %lnbXk = ptrtoint i64* %lnbXj to i64
  %lnbXl = inttoptr i64 %lnbXk to i64*
  store i64* %lnbXl, i64** %Sp_Var
  %lnbXm = bitcast i8* @stg_ap_0_fast to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbXn = load i64*, i64** %Sp_Var
  %lnbXo = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbXm( i64* %Base_Arg, i64* %lnbXn, i64* %Hp_Arg, i64 %lnbXo, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbVS:
  %lnbXr = load i64, i64* %R1_Var
  %lnbXs = add i64 %lnbXr, 6
  %lnbXt = inttoptr i64 %lnbXs to i64*
  %lnbXu = load i64, i64* %lnbXt, !tbaa !3
  store i64 %lnbXu, i64* %R1_Var
  %lnbXv = load i64*, i64** %Sp_Var
  %lnbXw = getelementptr inbounds i64, i64* %lnbXv, i32 1
  %lnbXx = ptrtoint i64* %lnbXw to i64
  %lnbXy = inttoptr i64 %lnbXx to i64*
  store i64* %lnbXy, i64** %Sp_Var
  %lnbXz = load i64*, i64** %Sp_Var
  %lnbXA = getelementptr inbounds i64, i64* %lnbXz, i32 0
  %lnbXB = bitcast i64* %lnbXA to i64*
  %lnbXC = load i64, i64* %lnbXB, !tbaa !1
  %lnbXD = inttoptr i64 %lnbXC to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbXE = load i64*, i64** %Sp_Var
  %lnbXF = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbXD( i64* %Base_Arg, i64* %lnbXE, i64* %Hp_Arg, i64 %lnbXF, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_varzuget_closure_struct = type <{i64, i64}>
@Disk_varzuget_closure$def = internal global %Disk_varzuget_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzuget_info$def to i64), i64 0}>
@Disk_varzuget_closure = alias i8* bitcast (%Disk_varzuget_closure_struct* @Disk_varzuget_closure$def to i8*)
@Disk_varzuget_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzuget_info$def to i8*)
define ghccc void @Disk_varzuget_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzuget_info$def to i64)),i64 112), i64 12884901903, i64 0, i64 4294967311}>
{
cbXL:
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnbXO = load i64, i64* %R3_Var
  store i64 %lnbXO, i64* %R3_Var
  %lnbXP = load i64, i64* %R2_Var
  store i64 %lnbXP, i64* %R2_Var
  %lnbXQ = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_varzuget1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbXR = load i64, i64* %R2_Var
  %lnbXS = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbXQ( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnbXR, i64 %lnbXS, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_printzustats2_closure_struct = type <{i64, i64, i64, i64}>
%cbY0_str_struct = type <{[16 x i8]}>
@Disk_printzustats2_closure$def = internal global %Disk_printzustats2_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_printzustats2_info$def to i64), i64 0, i64 0, i64 0}>
@Disk_printzustats2_closure = alias i8* bitcast (%Disk_printzustats2_closure_struct* @Disk_printzustats2_closure$def to i8*)
@cbY0_str$def = internal constant %cbY0_str_struct<{[16 x i8] [i8 68, i8 105, i8 115, i8 107, i8 32, i8 73, i8 47, i8 79, i8 32, i8 115, i8 116, i8 97, i8 116, i8 115, i8 58, i8 0]}>
@cbY0_str = internal alias i8* bitcast (%cbY0_str_struct* @cbY0_str$def to i8*)
@Disk_printzustats2_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_printzustats2_info$def to i8*)
define ghccc void @Disk_printzustats2_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 22}>
{
cbY1:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %lcbXX = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnbY4 = load i64*, i64** %Sp_Var
  %lnbY5 = getelementptr inbounds i64, i64* %lnbY4, i32 -2
  %lnbY6 = ptrtoint i64* %lnbY5 to i64
  %lnbY7 = icmp ult i64 %lnbY6, %SpLim_Arg
  br i1 %lnbY7, label %cbY2, label %cbY3
cbY3:
  %lnbY8 = ptrtoint i64* %Base_Arg to i64
  %lnbY9 = inttoptr i64 %lnbY8 to i8*
  %lnbYa = load i64, i64* %R1_Var
  %lnbYb = inttoptr i64 %lnbYa to i8*
  %lnbYc = bitcast i8* @newCAF to i8* (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lnbYd = call ccc i8* (i8*, i8*) %lnbYc( i8* %lnbY9, i8* %lnbYb ) nounwind
  %lnbYe = ptrtoint i8* %lnbYd to i64
  store i64 %lnbYe, i64* %lcbXX
  %lnbYf = load i64, i64* %lcbXX
  %lnbYg = icmp eq i64 %lnbYf, 0
  br i1 %lnbYg, label %cbXZ, label %cbXY
cbXY:
  %lnbYi = ptrtoint i8* @stg_bh_upd_frame_info to i64
  %lnbYh = load i64*, i64** %Sp_Var
  %lnbYj = getelementptr inbounds i64, i64* %lnbYh, i32 -2
  store i64 %lnbYi, i64* %lnbYj, !tbaa !1
  %lnbYl = load i64, i64* %lcbXX
  %lnbYk = load i64*, i64** %Sp_Var
  %lnbYm = getelementptr inbounds i64, i64* %lnbYk, i32 -1
  store i64 %lnbYl, i64* %lnbYm, !tbaa !1
  %lnbYn = ptrtoint %cbY0_str_struct* @cbY0_str$def to i64
  store i64 %lnbYn, i64* %R2_Var
  %lnbYo = load i64*, i64** %Sp_Var
  %lnbYp = getelementptr inbounds i64, i64* %lnbYo, i32 -2
  %lnbYq = ptrtoint i64* %lnbYp to i64
  %lnbYr = inttoptr i64 %lnbYq to i64*
  store i64* %lnbYr, i64** %Sp_Var
  %lnbYs = bitcast i8* @ghczmprim_GHCziCString_unpackCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbYt = load i64*, i64** %Sp_Var
  %lnbYu = load i64, i64* %R1_Var
  %lnbYv = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbYs( i64* %Base_Arg, i64* %lnbYt, i64* %Hp_Arg, i64 %lnbYu, i64 %lnbYv, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbXZ:
  %lnbYx = load i64, i64* %R1_Var
  %lnbYy = inttoptr i64 %lnbYx to i64*
  %lnbYz = load i64, i64* %lnbYy, !tbaa !3
  %lnbYA = inttoptr i64 %lnbYz to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbYB = load i64*, i64** %Sp_Var
  %lnbYC = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbYA( i64* %Base_Arg, i64* %lnbYB, i64* %Hp_Arg, i64 %lnbYC, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbY2:
  %lnbYD = load i64, i64* %R1_Var
  store i64 %lnbYD, i64* %R1_Var
  %lnbYE = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lnbYF = bitcast i64* %lnbYE to i64*
  %lnbYG = load i64, i64* %lnbYF, !tbaa !4
  %lnbYH = inttoptr i64 %lnbYG to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnbYI = load i64*, i64** %Sp_Var
  %lnbYJ = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnbYH( i64* %Base_Arg, i64* %lnbYI, i64* %Hp_Arg, i64 %lnbYJ, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_zdwprintzustats_closure_struct = type <{i64, i64}>
%cbZ8_str_struct = type <{[9 x i8]}>
%cbZx_str_struct = type <{[9 x i8]}>
%cbZW_str_struct = type <{[9 x i8]}>
@Disk_zdwprintzustats_closure$def = internal global %Disk_zdwprintzustats_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwprintzustats_info$def to i64), i64 0}>
@Disk_zdwprintzustats_closure = alias i8* bitcast (%Disk_zdwprintzustats_closure_struct* @Disk_zdwprintzustats_closure$def to i8*)
@cbZ8_str$def = internal constant %cbZ8_str_struct<{[9 x i8] [i8 82, i8 101, i8 97, i8 100, i8 115, i8 58, i8 32, i8 32, i8 0]}>
@cbZ8_str = internal alias i8* bitcast (%cbZ8_str_struct* @cbZ8_str$def to i8*)
@cbZx_str$def = internal constant %cbZx_str_struct<{[9 x i8] [i8 87, i8 114, i8 105, i8 116, i8 101, i8 115, i8 58, i8 32, i8 0]}>
@cbZx_str = internal alias i8* bitcast (%cbZx_str_struct* @cbZx_str$def to i8*)
@cbZW_str$def = internal constant %cbZW_str_struct<{[9 x i8] [i8 83, i8 121, i8 110, i8 99, i8 115, i8 58, i8 32, i8 32, i8 0]}>
@cbZW_str = internal alias i8* bitcast (%cbZW_str_struct* @cbZW_str$def to i8*)
@sb3o_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb3o_info$def to i8*)
define internal ghccc void @sb3o_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 4294967296, i64 18}>
{
cbZ2:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnc0e = load i64*, i64** %Sp_Var
  %lnc0f = getelementptr inbounds i64, i64* %lnc0e, i32 -3
  %lnc0g = ptrtoint i64* %lnc0f to i64
  %lnc0h = icmp ult i64 %lnc0g, %SpLim_Arg
  br i1 %lnc0h, label %cbZ3, label %cbZ4
cbZ4:
  %lnc0j = ptrtoint i8* @stg_upd_frame_info to i64
  %lnc0i = load i64*, i64** %Sp_Var
  %lnc0k = getelementptr inbounds i64, i64* %lnc0i, i32 -2
  store i64 %lnc0j, i64* %lnc0k, !tbaa !1
  %lnc0m = load i64, i64* %R1_Var
  %lnc0l = load i64*, i64** %Sp_Var
  %lnc0n = getelementptr inbounds i64, i64* %lnc0l, i32 -1
  store i64 %lnc0m, i64* %lnc0n, !tbaa !1
  %lnc0p = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbYZ_info$def to i64
  %lnc0o = load i64*, i64** %Sp_Var
  %lnc0q = getelementptr inbounds i64, i64* %lnc0o, i32 -3
  store i64 %lnc0p, i64* %lnc0q, !tbaa !1
  %lnc0r = ptrtoint i8* @ghczmprim_GHCziTypes_ZMZN_closure to i64
  %lnc0s = add i64 %lnc0r, 1
  store i64 %lnc0s, i64* %R3_Var
  %lnc0v = load i64, i64* %R1_Var
  %lnc0w = add i64 %lnc0v, 16
  %lnc0x = inttoptr i64 %lnc0w to i64*
  %lnc0y = load i64, i64* %lnc0x, !tbaa !3
  store i64 %lnc0y, i64* %R2_Var
  %lnc0z = load i64*, i64** %Sp_Var
  %lnc0A = getelementptr inbounds i64, i64* %lnc0z, i32 -3
  %lnc0B = ptrtoint i64* %lnc0A to i64
  %lnc0C = inttoptr i64 %lnc0B to i64*
  store i64* %lnc0C, i64** %Sp_Var
  %lnc0D = bitcast i8* @base_GHCziShow_zdwshowWord_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc0E = load i64*, i64** %Sp_Var
  %lnc0F = load i64, i64* %R1_Var
  %lnc0G = load i64, i64* %R2_Var
  %lnc0H = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc0D( i64* %Base_Arg, i64* %lnc0E, i64* %Hp_Arg, i64 %lnc0F, i64 %lnc0G, i64 %lnc0H, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbZ3:
  %lnc0I = load i64, i64* %R1_Var
  store i64 %lnc0I, i64* %R1_Var
  %lnc0J = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lnc0K = bitcast i64* %lnc0J to i64*
  %lnc0L = load i64, i64* %lnc0K, !tbaa !4
  %lnc0M = inttoptr i64 %lnc0L to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc0N = load i64*, i64** %Sp_Var
  %lnc0O = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc0M( i64* %Base_Arg, i64* %lnc0N, i64* %Hp_Arg, i64 %lnc0O, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbYZ_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbYZ_info$def to i8*)
define internal ghccc void @cbYZ_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
cbYZ:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnc0P = load i64*, i64** %Hp_Var
  %lnc0Q = getelementptr inbounds i64, i64* %lnc0P, i32 3
  %lnc0R = ptrtoint i64* %lnc0Q to i64
  %lnc0S = inttoptr i64 %lnc0R to i64*
  store i64* %lnc0S, i64** %Hp_Var
  %lnc0T = load i64*, i64** %Hp_Var
  %lnc0U = ptrtoint i64* %lnc0T to i64
  %lnc0V = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnc0W = bitcast i64* %lnc0V to i64*
  %lnc0X = load i64, i64* %lnc0W, !tbaa !4
  %lnc0Y = icmp ugt i64 %lnc0U, %lnc0X
  br i1 %lnc0Y, label %cbZ7, label %cbZ6
cbZ6:
  %lnc10 = ptrtoint i8* @ghczmprim_GHCziTypes_ZC_con_info to i64
  %lnc0Z = load i64*, i64** %Hp_Var
  %lnc11 = getelementptr inbounds i64, i64* %lnc0Z, i32 -2
  store i64 %lnc10, i64* %lnc11, !tbaa !2
  %lnc13 = load i64, i64* %R1_Var
  %lnc12 = load i64*, i64** %Hp_Var
  %lnc14 = getelementptr inbounds i64, i64* %lnc12, i32 -1
  store i64 %lnc13, i64* %lnc14, !tbaa !2
  %lnc16 = load i64, i64* %R2_Var
  %lnc15 = load i64*, i64** %Hp_Var
  %lnc17 = getelementptr inbounds i64, i64* %lnc15, i32 0
  store i64 %lnc16, i64* %lnc17, !tbaa !2
  %lnc19 = load i64*, i64** %Hp_Var
  %lnc1a = ptrtoint i64* %lnc19 to i64
  %lnc1b = add i64 %lnc1a, -14
  store i64 %lnc1b, i64* %R1_Var
  %lnc1c = load i64*, i64** %Sp_Var
  %lnc1d = getelementptr inbounds i64, i64* %lnc1c, i32 1
  %lnc1e = ptrtoint i64* %lnc1d to i64
  %lnc1f = inttoptr i64 %lnc1e to i64*
  store i64* %lnc1f, i64** %Sp_Var
  %lnc1g = load i64*, i64** %Sp_Var
  %lnc1h = getelementptr inbounds i64, i64* %lnc1g, i32 0
  %lnc1i = bitcast i64* %lnc1h to i64*
  %lnc1j = load i64, i64* %lnc1i, !tbaa !1
  %lnc1k = inttoptr i64 %lnc1j to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc1l = load i64*, i64** %Sp_Var
  %lnc1m = load i64*, i64** %Hp_Var
  %lnc1n = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc1k( i64* %Base_Arg, i64* %lnc1l, i64* %lnc1m, i64 %lnc1n, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbZ7:
  %lnc1o = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lnc1o, !tbaa !4
  %lnc1p = load i64, i64* %R2_Var
  store i64 %lnc1p, i64* %R2_Var
  %lnc1q = load i64, i64* %R1_Var
  store i64 %lnc1q, i64* %R1_Var
  %lnc1r = bitcast i8* @stg_gc_pp to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc1s = load i64*, i64** %Sp_Var
  %lnc1t = load i64*, i64** %Hp_Var
  %lnc1u = load i64, i64* %R1_Var
  %lnc1v = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc1r( i64* %Base_Arg, i64* %lnc1s, i64* %lnc1t, i64 %lnc1u, i64 %lnc1v, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@sb3p_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb3p_info$def to i8*)
define internal ghccc void @sb3p_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 4294967296, i64 18}>
{
cbZ9:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb3e = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnc1w = load i64*, i64** %Hp_Var
  %lnc1x = getelementptr inbounds i64, i64* %lnc1w, i32 3
  %lnc1y = ptrtoint i64* %lnc1x to i64
  %lnc1z = inttoptr i64 %lnc1y to i64*
  store i64* %lnc1z, i64** %Hp_Var
  %lnc1A = load i64*, i64** %Hp_Var
  %lnc1B = ptrtoint i64* %lnc1A to i64
  %lnc1C = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnc1D = bitcast i64* %lnc1C to i64*
  %lnc1E = load i64, i64* %lnc1D, !tbaa !4
  %lnc1F = icmp ugt i64 %lnc1B, %lnc1E
  br i1 %lnc1F, label %cbZd, label %cbZc
cbZc:
  %lnc1I = load i64, i64* %R1_Var
  %lnc1J = add i64 %lnc1I, 16
  %lnc1K = inttoptr i64 %lnc1J to i64*
  %lnc1L = load i64, i64* %lnc1K, !tbaa !3
  store i64 %lnc1L, i64* %lsb3e
  %lnc1N = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb3o_info$def to i64
  %lnc1M = load i64*, i64** %Hp_Var
  %lnc1O = getelementptr inbounds i64, i64* %lnc1M, i32 -2
  store i64 %lnc1N, i64* %lnc1O, !tbaa !2
  %lnc1Q = load i64, i64* %lsb3e
  %lnc1P = load i64*, i64** %Hp_Var
  %lnc1R = getelementptr inbounds i64, i64* %lnc1P, i32 0
  store i64 %lnc1Q, i64* %lnc1R, !tbaa !2
  %lnc1S = load i64*, i64** %Hp_Var
  %lnc1T = getelementptr inbounds i64, i64* %lnc1S, i32 -2
  %lnc1U = ptrtoint i64* %lnc1T to i64
  store i64 %lnc1U, i64* %R3_Var
  %lnc1V = ptrtoint %cbZ8_str_struct* @cbZ8_str$def to i64
  store i64 %lnc1V, i64* %R2_Var
  %lnc1W = bitcast i8* @ghczmprim_GHCziCString_unpackAppendCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc1X = load i64*, i64** %Hp_Var
  %lnc1Y = load i64, i64* %R1_Var
  %lnc1Z = load i64, i64* %R2_Var
  %lnc20 = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc1W( i64* %Base_Arg, i64* %Sp_Arg, i64* %lnc1X, i64 %lnc1Y, i64 %lnc1Z, i64 %lnc20, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbZd:
  %lnc21 = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lnc21, !tbaa !4
  %lnc22 = load i64, i64* %R1_Var
  store i64 %lnc22, i64* %R1_Var
  %lnc23 = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lnc24 = bitcast i64* %lnc23 to i64*
  %lnc25 = load i64, i64* %lnc24, !tbaa !4
  %lnc26 = inttoptr i64 %lnc25 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc27 = load i64*, i64** %Hp_Var
  %lnc28 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc26( i64* %Base_Arg, i64* %Sp_Arg, i64* %lnc27, i64 %lnc28, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@sb3w_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb3w_info$def to i8*)
define internal ghccc void @sb3w_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 4294967296, i64 18}>
{
cbZr:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnc29 = load i64*, i64** %Sp_Var
  %lnc2a = getelementptr inbounds i64, i64* %lnc29, i32 -3
  %lnc2b = ptrtoint i64* %lnc2a to i64
  %lnc2c = icmp ult i64 %lnc2b, %SpLim_Arg
  br i1 %lnc2c, label %cbZs, label %cbZt
cbZt:
  %lnc2e = ptrtoint i8* @stg_upd_frame_info to i64
  %lnc2d = load i64*, i64** %Sp_Var
  %lnc2f = getelementptr inbounds i64, i64* %lnc2d, i32 -2
  store i64 %lnc2e, i64* %lnc2f, !tbaa !1
  %lnc2h = load i64, i64* %R1_Var
  %lnc2g = load i64*, i64** %Sp_Var
  %lnc2i = getelementptr inbounds i64, i64* %lnc2g, i32 -1
  store i64 %lnc2h, i64* %lnc2i, !tbaa !1
  %lnc2k = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbZo_info$def to i64
  %lnc2j = load i64*, i64** %Sp_Var
  %lnc2l = getelementptr inbounds i64, i64* %lnc2j, i32 -3
  store i64 %lnc2k, i64* %lnc2l, !tbaa !1
  %lnc2m = ptrtoint i8* @ghczmprim_GHCziTypes_ZMZN_closure to i64
  %lnc2n = add i64 %lnc2m, 1
  store i64 %lnc2n, i64* %R3_Var
  %lnc2q = load i64, i64* %R1_Var
  %lnc2r = add i64 %lnc2q, 16
  %lnc2s = inttoptr i64 %lnc2r to i64*
  %lnc2t = load i64, i64* %lnc2s, !tbaa !3
  store i64 %lnc2t, i64* %R2_Var
  %lnc2u = load i64*, i64** %Sp_Var
  %lnc2v = getelementptr inbounds i64, i64* %lnc2u, i32 -3
  %lnc2w = ptrtoint i64* %lnc2v to i64
  %lnc2x = inttoptr i64 %lnc2w to i64*
  store i64* %lnc2x, i64** %Sp_Var
  %lnc2y = bitcast i8* @base_GHCziShow_zdwshowWord_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc2z = load i64*, i64** %Sp_Var
  %lnc2A = load i64, i64* %R1_Var
  %lnc2B = load i64, i64* %R2_Var
  %lnc2C = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc2y( i64* %Base_Arg, i64* %lnc2z, i64* %Hp_Arg, i64 %lnc2A, i64 %lnc2B, i64 %lnc2C, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbZs:
  %lnc2D = load i64, i64* %R1_Var
  store i64 %lnc2D, i64* %R1_Var
  %lnc2E = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lnc2F = bitcast i64* %lnc2E to i64*
  %lnc2G = load i64, i64* %lnc2F, !tbaa !4
  %lnc2H = inttoptr i64 %lnc2G to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc2I = load i64*, i64** %Sp_Var
  %lnc2J = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc2H( i64* %Base_Arg, i64* %lnc2I, i64* %Hp_Arg, i64 %lnc2J, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbZo_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbZo_info$def to i8*)
define internal ghccc void @cbZo_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
cbZo:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnc2K = load i64*, i64** %Hp_Var
  %lnc2L = getelementptr inbounds i64, i64* %lnc2K, i32 3
  %lnc2M = ptrtoint i64* %lnc2L to i64
  %lnc2N = inttoptr i64 %lnc2M to i64*
  store i64* %lnc2N, i64** %Hp_Var
  %lnc2O = load i64*, i64** %Hp_Var
  %lnc2P = ptrtoint i64* %lnc2O to i64
  %lnc2Q = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnc2R = bitcast i64* %lnc2Q to i64*
  %lnc2S = load i64, i64* %lnc2R, !tbaa !4
  %lnc2T = icmp ugt i64 %lnc2P, %lnc2S
  br i1 %lnc2T, label %cbZw, label %cbZv
cbZv:
  %lnc2V = ptrtoint i8* @ghczmprim_GHCziTypes_ZC_con_info to i64
  %lnc2U = load i64*, i64** %Hp_Var
  %lnc2W = getelementptr inbounds i64, i64* %lnc2U, i32 -2
  store i64 %lnc2V, i64* %lnc2W, !tbaa !2
  %lnc2Y = load i64, i64* %R1_Var
  %lnc2X = load i64*, i64** %Hp_Var
  %lnc2Z = getelementptr inbounds i64, i64* %lnc2X, i32 -1
  store i64 %lnc2Y, i64* %lnc2Z, !tbaa !2
  %lnc31 = load i64, i64* %R2_Var
  %lnc30 = load i64*, i64** %Hp_Var
  %lnc32 = getelementptr inbounds i64, i64* %lnc30, i32 0
  store i64 %lnc31, i64* %lnc32, !tbaa !2
  %lnc34 = load i64*, i64** %Hp_Var
  %lnc35 = ptrtoint i64* %lnc34 to i64
  %lnc36 = add i64 %lnc35, -14
  store i64 %lnc36, i64* %R1_Var
  %lnc37 = load i64*, i64** %Sp_Var
  %lnc38 = getelementptr inbounds i64, i64* %lnc37, i32 1
  %lnc39 = ptrtoint i64* %lnc38 to i64
  %lnc3a = inttoptr i64 %lnc39 to i64*
  store i64* %lnc3a, i64** %Sp_Var
  %lnc3b = load i64*, i64** %Sp_Var
  %lnc3c = getelementptr inbounds i64, i64* %lnc3b, i32 0
  %lnc3d = bitcast i64* %lnc3c to i64*
  %lnc3e = load i64, i64* %lnc3d, !tbaa !1
  %lnc3f = inttoptr i64 %lnc3e to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc3g = load i64*, i64** %Sp_Var
  %lnc3h = load i64*, i64** %Hp_Var
  %lnc3i = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc3f( i64* %Base_Arg, i64* %lnc3g, i64* %lnc3h, i64 %lnc3i, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbZw:
  %lnc3j = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lnc3j, !tbaa !4
  %lnc3k = load i64, i64* %R2_Var
  store i64 %lnc3k, i64* %R2_Var
  %lnc3l = load i64, i64* %R1_Var
  store i64 %lnc3l, i64* %R1_Var
  %lnc3m = bitcast i8* @stg_gc_pp to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc3n = load i64*, i64** %Sp_Var
  %lnc3o = load i64*, i64** %Hp_Var
  %lnc3p = load i64, i64* %R1_Var
  %lnc3q = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc3m( i64* %Base_Arg, i64* %lnc3n, i64* %lnc3o, i64 %lnc3p, i64 %lnc3q, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@sb3x_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb3x_info$def to i8*)
define internal ghccc void @sb3x_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 4294967296, i64 18}>
{
cbZy:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb3f = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnc3r = load i64*, i64** %Hp_Var
  %lnc3s = getelementptr inbounds i64, i64* %lnc3r, i32 3
  %lnc3t = ptrtoint i64* %lnc3s to i64
  %lnc3u = inttoptr i64 %lnc3t to i64*
  store i64* %lnc3u, i64** %Hp_Var
  %lnc3v = load i64*, i64** %Hp_Var
  %lnc3w = ptrtoint i64* %lnc3v to i64
  %lnc3x = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnc3y = bitcast i64* %lnc3x to i64*
  %lnc3z = load i64, i64* %lnc3y, !tbaa !4
  %lnc3A = icmp ugt i64 %lnc3w, %lnc3z
  br i1 %lnc3A, label %cbZC, label %cbZB
cbZB:
  %lnc3D = load i64, i64* %R1_Var
  %lnc3E = add i64 %lnc3D, 16
  %lnc3F = inttoptr i64 %lnc3E to i64*
  %lnc3G = load i64, i64* %lnc3F, !tbaa !3
  store i64 %lnc3G, i64* %lsb3f
  %lnc3I = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb3w_info$def to i64
  %lnc3H = load i64*, i64** %Hp_Var
  %lnc3J = getelementptr inbounds i64, i64* %lnc3H, i32 -2
  store i64 %lnc3I, i64* %lnc3J, !tbaa !2
  %lnc3L = load i64, i64* %lsb3f
  %lnc3K = load i64*, i64** %Hp_Var
  %lnc3M = getelementptr inbounds i64, i64* %lnc3K, i32 0
  store i64 %lnc3L, i64* %lnc3M, !tbaa !2
  %lnc3N = load i64*, i64** %Hp_Var
  %lnc3O = getelementptr inbounds i64, i64* %lnc3N, i32 -2
  %lnc3P = ptrtoint i64* %lnc3O to i64
  store i64 %lnc3P, i64* %R3_Var
  %lnc3Q = ptrtoint %cbZx_str_struct* @cbZx_str$def to i64
  store i64 %lnc3Q, i64* %R2_Var
  %lnc3R = bitcast i8* @ghczmprim_GHCziCString_unpackAppendCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc3S = load i64*, i64** %Hp_Var
  %lnc3T = load i64, i64* %R1_Var
  %lnc3U = load i64, i64* %R2_Var
  %lnc3V = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc3R( i64* %Base_Arg, i64* %Sp_Arg, i64* %lnc3S, i64 %lnc3T, i64 %lnc3U, i64 %lnc3V, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbZC:
  %lnc3W = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lnc3W, !tbaa !4
  %lnc3X = load i64, i64* %R1_Var
  store i64 %lnc3X, i64* %R1_Var
  %lnc3Y = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lnc3Z = bitcast i64* %lnc3Y to i64*
  %lnc40 = load i64, i64* %lnc3Z, !tbaa !4
  %lnc41 = inttoptr i64 %lnc40 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc42 = load i64*, i64** %Hp_Var
  %lnc43 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc41( i64* %Base_Arg, i64* %Sp_Arg, i64* %lnc42, i64 %lnc43, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@sb3E_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb3E_info$def to i8*)
define internal ghccc void @sb3E_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 4294967296, i64 18}>
{
cbZQ:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnc44 = load i64*, i64** %Sp_Var
  %lnc45 = getelementptr inbounds i64, i64* %lnc44, i32 -3
  %lnc46 = ptrtoint i64* %lnc45 to i64
  %lnc47 = icmp ult i64 %lnc46, %SpLim_Arg
  br i1 %lnc47, label %cbZR, label %cbZS
cbZS:
  %lnc49 = ptrtoint i8* @stg_upd_frame_info to i64
  %lnc48 = load i64*, i64** %Sp_Var
  %lnc4a = getelementptr inbounds i64, i64* %lnc48, i32 -2
  store i64 %lnc49, i64* %lnc4a, !tbaa !1
  %lnc4c = load i64, i64* %R1_Var
  %lnc4b = load i64*, i64** %Sp_Var
  %lnc4d = getelementptr inbounds i64, i64* %lnc4b, i32 -1
  store i64 %lnc4c, i64* %lnc4d, !tbaa !1
  %lnc4f = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbZN_info$def to i64
  %lnc4e = load i64*, i64** %Sp_Var
  %lnc4g = getelementptr inbounds i64, i64* %lnc4e, i32 -3
  store i64 %lnc4f, i64* %lnc4g, !tbaa !1
  %lnc4h = ptrtoint i8* @ghczmprim_GHCziTypes_ZMZN_closure to i64
  %lnc4i = add i64 %lnc4h, 1
  store i64 %lnc4i, i64* %R3_Var
  %lnc4l = load i64, i64* %R1_Var
  %lnc4m = add i64 %lnc4l, 16
  %lnc4n = inttoptr i64 %lnc4m to i64*
  %lnc4o = load i64, i64* %lnc4n, !tbaa !3
  store i64 %lnc4o, i64* %R2_Var
  %lnc4p = load i64*, i64** %Sp_Var
  %lnc4q = getelementptr inbounds i64, i64* %lnc4p, i32 -3
  %lnc4r = ptrtoint i64* %lnc4q to i64
  %lnc4s = inttoptr i64 %lnc4r to i64*
  store i64* %lnc4s, i64** %Sp_Var
  %lnc4t = bitcast i8* @base_GHCziShow_zdwshowWord_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc4u = load i64*, i64** %Sp_Var
  %lnc4v = load i64, i64* %R1_Var
  %lnc4w = load i64, i64* %R2_Var
  %lnc4x = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc4t( i64* %Base_Arg, i64* %lnc4u, i64* %Hp_Arg, i64 %lnc4v, i64 %lnc4w, i64 %lnc4x, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbZR:
  %lnc4y = load i64, i64* %R1_Var
  store i64 %lnc4y, i64* %R1_Var
  %lnc4z = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lnc4A = bitcast i64* %lnc4z to i64*
  %lnc4B = load i64, i64* %lnc4A, !tbaa !4
  %lnc4C = inttoptr i64 %lnc4B to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc4D = load i64*, i64** %Sp_Var
  %lnc4E = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc4C( i64* %Base_Arg, i64* %lnc4D, i64* %Hp_Arg, i64 %lnc4E, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbZN_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbZN_info$def to i8*)
define internal ghccc void @cbZN_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
cbZN:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnc4F = load i64*, i64** %Hp_Var
  %lnc4G = getelementptr inbounds i64, i64* %lnc4F, i32 3
  %lnc4H = ptrtoint i64* %lnc4G to i64
  %lnc4I = inttoptr i64 %lnc4H to i64*
  store i64* %lnc4I, i64** %Hp_Var
  %lnc4J = load i64*, i64** %Hp_Var
  %lnc4K = ptrtoint i64* %lnc4J to i64
  %lnc4L = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnc4M = bitcast i64* %lnc4L to i64*
  %lnc4N = load i64, i64* %lnc4M, !tbaa !4
  %lnc4O = icmp ugt i64 %lnc4K, %lnc4N
  br i1 %lnc4O, label %cbZV, label %cbZU
cbZU:
  %lnc4Q = ptrtoint i8* @ghczmprim_GHCziTypes_ZC_con_info to i64
  %lnc4P = load i64*, i64** %Hp_Var
  %lnc4R = getelementptr inbounds i64, i64* %lnc4P, i32 -2
  store i64 %lnc4Q, i64* %lnc4R, !tbaa !2
  %lnc4T = load i64, i64* %R1_Var
  %lnc4S = load i64*, i64** %Hp_Var
  %lnc4U = getelementptr inbounds i64, i64* %lnc4S, i32 -1
  store i64 %lnc4T, i64* %lnc4U, !tbaa !2
  %lnc4W = load i64, i64* %R2_Var
  %lnc4V = load i64*, i64** %Hp_Var
  %lnc4X = getelementptr inbounds i64, i64* %lnc4V, i32 0
  store i64 %lnc4W, i64* %lnc4X, !tbaa !2
  %lnc4Z = load i64*, i64** %Hp_Var
  %lnc50 = ptrtoint i64* %lnc4Z to i64
  %lnc51 = add i64 %lnc50, -14
  store i64 %lnc51, i64* %R1_Var
  %lnc52 = load i64*, i64** %Sp_Var
  %lnc53 = getelementptr inbounds i64, i64* %lnc52, i32 1
  %lnc54 = ptrtoint i64* %lnc53 to i64
  %lnc55 = inttoptr i64 %lnc54 to i64*
  store i64* %lnc55, i64** %Sp_Var
  %lnc56 = load i64*, i64** %Sp_Var
  %lnc57 = getelementptr inbounds i64, i64* %lnc56, i32 0
  %lnc58 = bitcast i64* %lnc57 to i64*
  %lnc59 = load i64, i64* %lnc58, !tbaa !1
  %lnc5a = inttoptr i64 %lnc59 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc5b = load i64*, i64** %Sp_Var
  %lnc5c = load i64*, i64** %Hp_Var
  %lnc5d = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc5a( i64* %Base_Arg, i64* %lnc5b, i64* %lnc5c, i64 %lnc5d, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cbZV:
  %lnc5e = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lnc5e, !tbaa !4
  %lnc5f = load i64, i64* %R2_Var
  store i64 %lnc5f, i64* %R2_Var
  %lnc5g = load i64, i64* %R1_Var
  store i64 %lnc5g, i64* %R1_Var
  %lnc5h = bitcast i8* @stg_gc_pp to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc5i = load i64*, i64** %Sp_Var
  %lnc5j = load i64*, i64** %Hp_Var
  %lnc5k = load i64, i64* %R1_Var
  %lnc5l = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc5h( i64* %Base_Arg, i64* %lnc5i, i64* %lnc5j, i64 %lnc5k, i64 %lnc5l, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@sb3F_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb3F_info$def to i8*)
define internal ghccc void @sb3F_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 4294967296, i64 18}>
{
cbZX:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb3g = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnc5m = load i64*, i64** %Hp_Var
  %lnc5n = getelementptr inbounds i64, i64* %lnc5m, i32 3
  %lnc5o = ptrtoint i64* %lnc5n to i64
  %lnc5p = inttoptr i64 %lnc5o to i64*
  store i64* %lnc5p, i64** %Hp_Var
  %lnc5q = load i64*, i64** %Hp_Var
  %lnc5r = ptrtoint i64* %lnc5q to i64
  %lnc5s = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnc5t = bitcast i64* %lnc5s to i64*
  %lnc5u = load i64, i64* %lnc5t, !tbaa !4
  %lnc5v = icmp ugt i64 %lnc5r, %lnc5u
  br i1 %lnc5v, label %cc01, label %cc00
cc00:
  %lnc5y = load i64, i64* %R1_Var
  %lnc5z = add i64 %lnc5y, 16
  %lnc5A = inttoptr i64 %lnc5z to i64*
  %lnc5B = load i64, i64* %lnc5A, !tbaa !3
  store i64 %lnc5B, i64* %lsb3g
  %lnc5D = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb3E_info$def to i64
  %lnc5C = load i64*, i64** %Hp_Var
  %lnc5E = getelementptr inbounds i64, i64* %lnc5C, i32 -2
  store i64 %lnc5D, i64* %lnc5E, !tbaa !2
  %lnc5G = load i64, i64* %lsb3g
  %lnc5F = load i64*, i64** %Hp_Var
  %lnc5H = getelementptr inbounds i64, i64* %lnc5F, i32 0
  store i64 %lnc5G, i64* %lnc5H, !tbaa !2
  %lnc5I = load i64*, i64** %Hp_Var
  %lnc5J = getelementptr inbounds i64, i64* %lnc5I, i32 -2
  %lnc5K = ptrtoint i64* %lnc5J to i64
  store i64 %lnc5K, i64* %R3_Var
  %lnc5L = ptrtoint %cbZW_str_struct* @cbZW_str$def to i64
  store i64 %lnc5L, i64* %R2_Var
  %lnc5M = bitcast i8* @ghczmprim_GHCziCString_unpackAppendCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc5N = load i64*, i64** %Hp_Var
  %lnc5O = load i64, i64* %R1_Var
  %lnc5P = load i64, i64* %R2_Var
  %lnc5Q = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc5M( i64* %Base_Arg, i64* %Sp_Arg, i64* %lnc5N, i64 %lnc5O, i64 %lnc5P, i64 %lnc5Q, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cc01:
  %lnc5R = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lnc5R, !tbaa !4
  %lnc5S = load i64, i64* %R1_Var
  store i64 %lnc5S, i64* %R1_Var
  %lnc5T = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lnc5U = bitcast i64* %lnc5T to i64*
  %lnc5V = load i64, i64* %lnc5U, !tbaa !4
  %lnc5W = inttoptr i64 %lnc5V to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc5X = load i64*, i64** %Hp_Var
  %lnc5Y = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc5W( i64* %Base_Arg, i64* %Sp_Arg, i64* %lnc5X, i64 %lnc5Y, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@Disk_zdwprintzustats_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwprintzustats_info$def to i8*)
define ghccc void @Disk_zdwprintzustats_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwprintzustats_info$def to i64)),i64 120), i64 17179869200, i64 0, i64 64424509455}>
{
cc02:
  %lsb3g = alloca i64, i32 1
  %R4_Var = alloca i64, i32 1
  store i64 %R4_Arg, i64* %R4_Var
  %lsb3f = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %lsb3e = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnc5Z = load i64*, i64** %Sp_Var
  %lnc60 = getelementptr inbounds i64, i64* %lnc5Z, i32 -4
  %lnc61 = ptrtoint i64* %lnc60 to i64
  %lnc62 = icmp ult i64 %lnc61, %SpLim_Arg
  br i1 %lnc62, label %cc03, label %cc04
cc04:
  %lnc64 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbYP_info$def to i64
  %lnc63 = load i64*, i64** %Sp_Var
  %lnc65 = getelementptr inbounds i64, i64* %lnc63, i32 -4
  store i64 %lnc64, i64* %lnc65, !tbaa !1
  %lnc66 = load i64, i64* %R4_Var
  store i64 %lnc66, i64* %lsb3g
  %lnc67 = ptrtoint i8* @ghczmprim_GHCziTypes_True_closure to i64
  %lnc68 = add i64 %lnc67, 2
  store i64 %lnc68, i64* %R4_Var
  %lnc69 = load i64, i64* %R3_Var
  store i64 %lnc69, i64* %lsb3f
  %lnc6a = ptrtoint %Disk_printzustats2_closure_struct* @Disk_printzustats2_closure$def to i64
  store i64 %lnc6a, i64* %R3_Var
  %lnc6b = load i64, i64* %R2_Var
  store i64 %lnc6b, i64* %lsb3e
  %lnc6c = ptrtoint i8* @base_GHCziIOziHandleziFD_stdout_closure to i64
  store i64 %lnc6c, i64* %R2_Var
  %lnc6e = load i64, i64* %lsb3e
  %lnc6d = load i64*, i64** %Sp_Var
  %lnc6f = getelementptr inbounds i64, i64* %lnc6d, i32 -3
  store i64 %lnc6e, i64* %lnc6f, !tbaa !1
  %lnc6h = load i64, i64* %lsb3f
  %lnc6g = load i64*, i64** %Sp_Var
  %lnc6i = getelementptr inbounds i64, i64* %lnc6g, i32 -2
  store i64 %lnc6h, i64* %lnc6i, !tbaa !1
  %lnc6k = load i64, i64* %lsb3g
  %lnc6j = load i64*, i64** %Sp_Var
  %lnc6l = getelementptr inbounds i64, i64* %lnc6j, i32 -1
  store i64 %lnc6k, i64* %lnc6l, !tbaa !1
  %lnc6m = load i64*, i64** %Sp_Var
  %lnc6n = getelementptr inbounds i64, i64* %lnc6m, i32 -4
  %lnc6o = ptrtoint i64* %lnc6n to i64
  %lnc6p = inttoptr i64 %lnc6o to i64*
  store i64* %lnc6p, i64** %Sp_Var
  %lnc6q = bitcast i8* @base_GHCziIOziHandleziText_hPutStr2_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc6r = load i64*, i64** %Sp_Var
  %lnc6s = load i64, i64* %R1_Var
  %lnc6t = load i64, i64* %R2_Var
  %lnc6u = load i64, i64* %R3_Var
  %lnc6v = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc6q( i64* %Base_Arg, i64* %lnc6r, i64* %Hp_Arg, i64 %lnc6s, i64 %lnc6t, i64 %lnc6u, i64 %lnc6v, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cc03:
  %lnc6w = load i64, i64* %R4_Var
  store i64 %lnc6w, i64* %R4_Var
  %lnc6x = load i64, i64* %R3_Var
  store i64 %lnc6x, i64* %R3_Var
  %lnc6y = load i64, i64* %R2_Var
  store i64 %lnc6y, i64* %R2_Var
  %lnc6z = ptrtoint %Disk_zdwprintzustats_closure_struct* @Disk_zdwprintzustats_closure$def to i64
  store i64 %lnc6z, i64* %R1_Var
  %lnc6A = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lnc6B = bitcast i64* %lnc6A to i64*
  %lnc6C = load i64, i64* %lnc6B, !tbaa !4
  %lnc6D = inttoptr i64 %lnc6C to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc6E = load i64*, i64** %Sp_Var
  %lnc6F = load i64, i64* %R1_Var
  %lnc6G = load i64, i64* %R2_Var
  %lnc6H = load i64, i64* %R3_Var
  %lnc6I = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc6D( i64* %Base_Arg, i64* %lnc6E, i64* %Hp_Arg, i64 %lnc6F, i64 %lnc6G, i64 %lnc6H, i64 %lnc6I, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbYP_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbYP_info$def to i8*)
define internal ghccc void @cbYP_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbYP_info$def to i64)),i64 120), i64 451, i64 38654705696}>
{
cbYP:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnc6J = load i64*, i64** %Hp_Var
  %lnc6K = getelementptr inbounds i64, i64* %lnc6J, i32 3
  %lnc6L = ptrtoint i64* %lnc6K to i64
  %lnc6M = inttoptr i64 %lnc6L to i64*
  store i64* %lnc6M, i64** %Hp_Var
  %lnc6N = load i64*, i64** %Hp_Var
  %lnc6O = ptrtoint i64* %lnc6N to i64
  %lnc6P = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnc6Q = bitcast i64* %lnc6P to i64*
  %lnc6R = load i64, i64* %lnc6Q, !tbaa !4
  %lnc6S = icmp ugt i64 %lnc6O, %lnc6R
  br i1 %lnc6S, label %cc07, label %cc06
cc06:
  %lnc6U = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb3p_info$def to i64
  %lnc6T = load i64*, i64** %Hp_Var
  %lnc6V = getelementptr inbounds i64, i64* %lnc6T, i32 -2
  store i64 %lnc6U, i64* %lnc6V, !tbaa !2
  %lnc6X = load i64*, i64** %Sp_Var
  %lnc6Y = getelementptr inbounds i64, i64* %lnc6X, i32 1
  %lnc6Z = bitcast i64* %lnc6Y to i64*
  %lnc70 = load i64, i64* %lnc6Z, !tbaa !1
  %lnc6W = load i64*, i64** %Hp_Var
  %lnc71 = getelementptr inbounds i64, i64* %lnc6W, i32 0
  store i64 %lnc70, i64* %lnc71, !tbaa !2
  %lnc73 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbZe_info$def to i64
  %lnc72 = load i64*, i64** %Sp_Var
  %lnc74 = getelementptr inbounds i64, i64* %lnc72, i32 1
  store i64 %lnc73, i64* %lnc74, !tbaa !1
  %lnc75 = ptrtoint i8* @ghczmprim_GHCziTypes_True_closure to i64
  %lnc76 = add i64 %lnc75, 2
  store i64 %lnc76, i64* %R4_Var
  %lnc77 = load i64*, i64** %Hp_Var
  %lnc78 = getelementptr inbounds i64, i64* %lnc77, i32 -2
  %lnc79 = ptrtoint i64* %lnc78 to i64
  store i64 %lnc79, i64* %R3_Var
  %lnc7a = ptrtoint i8* @base_GHCziIOziHandleziFD_stdout_closure to i64
  store i64 %lnc7a, i64* %R2_Var
  %lnc7b = load i64*, i64** %Sp_Var
  %lnc7c = getelementptr inbounds i64, i64* %lnc7b, i32 1
  %lnc7d = ptrtoint i64* %lnc7c to i64
  %lnc7e = inttoptr i64 %lnc7d to i64*
  store i64* %lnc7e, i64** %Sp_Var
  %lnc7f = bitcast i8* @base_GHCziIOziHandleziText_hPutStr2_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc7g = load i64*, i64** %Sp_Var
  %lnc7h = load i64*, i64** %Hp_Var
  %lnc7i = load i64, i64* %R1_Var
  %lnc7j = load i64, i64* %R2_Var
  %lnc7k = load i64, i64* %R3_Var
  %lnc7l = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc7f( i64* %Base_Arg, i64* %lnc7g, i64* %lnc7h, i64 %lnc7i, i64 %lnc7j, i64 %lnc7k, i64 %lnc7l, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cc07:
  %lnc7m = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lnc7m, !tbaa !4
  %lnc7n = load i64, i64* %R1_Var
  store i64 %lnc7n, i64* %R1_Var
  %lnc7o = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc7p = load i64*, i64** %Sp_Var
  %lnc7q = load i64*, i64** %Hp_Var
  %lnc7r = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc7o( i64* %Base_Arg, i64* %lnc7p, i64* %lnc7q, i64 %lnc7r, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbZe_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbZe_info$def to i8*)
define internal ghccc void @cbZe_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbZe_info$def to i64)),i64 120), i64 194, i64 38654705696}>
{
cbZe:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnc7s = load i64*, i64** %Hp_Var
  %lnc7t = getelementptr inbounds i64, i64* %lnc7s, i32 3
  %lnc7u = ptrtoint i64* %lnc7t to i64
  %lnc7v = inttoptr i64 %lnc7u to i64*
  store i64* %lnc7v, i64** %Hp_Var
  %lnc7w = load i64*, i64** %Hp_Var
  %lnc7x = ptrtoint i64* %lnc7w to i64
  %lnc7y = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnc7z = bitcast i64* %lnc7y to i64*
  %lnc7A = load i64, i64* %lnc7z, !tbaa !4
  %lnc7B = icmp ugt i64 %lnc7x, %lnc7A
  br i1 %lnc7B, label %cc0a, label %cc09
cc09:
  %lnc7D = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb3x_info$def to i64
  %lnc7C = load i64*, i64** %Hp_Var
  %lnc7E = getelementptr inbounds i64, i64* %lnc7C, i32 -2
  store i64 %lnc7D, i64* %lnc7E, !tbaa !2
  %lnc7G = load i64*, i64** %Sp_Var
  %lnc7H = getelementptr inbounds i64, i64* %lnc7G, i32 1
  %lnc7I = bitcast i64* %lnc7H to i64*
  %lnc7J = load i64, i64* %lnc7I, !tbaa !1
  %lnc7F = load i64*, i64** %Hp_Var
  %lnc7K = getelementptr inbounds i64, i64* %lnc7F, i32 0
  store i64 %lnc7J, i64* %lnc7K, !tbaa !2
  %lnc7M = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbZD_info$def to i64
  %lnc7L = load i64*, i64** %Sp_Var
  %lnc7N = getelementptr inbounds i64, i64* %lnc7L, i32 1
  store i64 %lnc7M, i64* %lnc7N, !tbaa !1
  %lnc7O = ptrtoint i8* @ghczmprim_GHCziTypes_True_closure to i64
  %lnc7P = add i64 %lnc7O, 2
  store i64 %lnc7P, i64* %R4_Var
  %lnc7Q = load i64*, i64** %Hp_Var
  %lnc7R = getelementptr inbounds i64, i64* %lnc7Q, i32 -2
  %lnc7S = ptrtoint i64* %lnc7R to i64
  store i64 %lnc7S, i64* %R3_Var
  %lnc7T = ptrtoint i8* @base_GHCziIOziHandleziFD_stdout_closure to i64
  store i64 %lnc7T, i64* %R2_Var
  %lnc7U = load i64*, i64** %Sp_Var
  %lnc7V = getelementptr inbounds i64, i64* %lnc7U, i32 1
  %lnc7W = ptrtoint i64* %lnc7V to i64
  %lnc7X = inttoptr i64 %lnc7W to i64*
  store i64* %lnc7X, i64** %Sp_Var
  %lnc7Y = bitcast i8* @base_GHCziIOziHandleziText_hPutStr2_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc7Z = load i64*, i64** %Sp_Var
  %lnc80 = load i64*, i64** %Hp_Var
  %lnc81 = load i64, i64* %R1_Var
  %lnc82 = load i64, i64* %R2_Var
  %lnc83 = load i64, i64* %R3_Var
  %lnc84 = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc7Y( i64* %Base_Arg, i64* %lnc7Z, i64* %lnc80, i64 %lnc81, i64 %lnc82, i64 %lnc83, i64 %lnc84, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cc0a:
  %lnc85 = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lnc85, !tbaa !4
  %lnc86 = load i64, i64* %R1_Var
  store i64 %lnc86, i64* %R1_Var
  %lnc87 = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc88 = load i64*, i64** %Sp_Var
  %lnc89 = load i64*, i64** %Hp_Var
  %lnc8a = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc87( i64* %Base_Arg, i64* %lnc88, i64* %lnc89, i64 %lnc8a, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cbZD_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbZD_info$def to i8*)
define internal ghccc void @cbZD_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cbZD_info$def to i64)),i64 120), i64 65, i64 38654705696}>
{
cbZD:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnc8b = load i64*, i64** %Hp_Var
  %lnc8c = getelementptr inbounds i64, i64* %lnc8b, i32 3
  %lnc8d = ptrtoint i64* %lnc8c to i64
  %lnc8e = inttoptr i64 %lnc8d to i64*
  store i64* %lnc8e, i64** %Hp_Var
  %lnc8f = load i64*, i64** %Hp_Var
  %lnc8g = ptrtoint i64* %lnc8f to i64
  %lnc8h = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnc8i = bitcast i64* %lnc8h to i64*
  %lnc8j = load i64, i64* %lnc8i, !tbaa !4
  %lnc8k = icmp ugt i64 %lnc8g, %lnc8j
  br i1 %lnc8k, label %cc0d, label %cc0c
cc0c:
  %lnc8m = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb3F_info$def to i64
  %lnc8l = load i64*, i64** %Hp_Var
  %lnc8n = getelementptr inbounds i64, i64* %lnc8l, i32 -2
  store i64 %lnc8m, i64* %lnc8n, !tbaa !2
  %lnc8p = load i64*, i64** %Sp_Var
  %lnc8q = getelementptr inbounds i64, i64* %lnc8p, i32 1
  %lnc8r = bitcast i64* %lnc8q to i64*
  %lnc8s = load i64, i64* %lnc8r, !tbaa !1
  %lnc8o = load i64*, i64** %Hp_Var
  %lnc8t = getelementptr inbounds i64, i64* %lnc8o, i32 0
  store i64 %lnc8s, i64* %lnc8t, !tbaa !2
  %lnc8u = ptrtoint i8* @ghczmprim_GHCziTypes_True_closure to i64
  %lnc8v = add i64 %lnc8u, 2
  store i64 %lnc8v, i64* %R4_Var
  %lnc8w = load i64*, i64** %Hp_Var
  %lnc8x = getelementptr inbounds i64, i64* %lnc8w, i32 -2
  %lnc8y = ptrtoint i64* %lnc8x to i64
  store i64 %lnc8y, i64* %R3_Var
  %lnc8z = ptrtoint i8* @base_GHCziIOziHandleziFD_stdout_closure to i64
  store i64 %lnc8z, i64* %R2_Var
  %lnc8A = load i64*, i64** %Sp_Var
  %lnc8B = getelementptr inbounds i64, i64* %lnc8A, i32 2
  %lnc8C = ptrtoint i64* %lnc8B to i64
  %lnc8D = inttoptr i64 %lnc8C to i64*
  store i64* %lnc8D, i64** %Sp_Var
  %lnc8E = bitcast i8* @base_GHCziIOziHandleziText_hPutStr2_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc8F = load i64*, i64** %Sp_Var
  %lnc8G = load i64*, i64** %Hp_Var
  %lnc8H = load i64, i64* %R1_Var
  %lnc8I = load i64, i64* %R2_Var
  %lnc8J = load i64, i64* %R3_Var
  %lnc8K = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc8E( i64* %Base_Arg, i64* %lnc8F, i64* %lnc8G, i64 %lnc8H, i64 %lnc8I, i64 %lnc8J, i64 %lnc8K, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cc0d:
  %lnc8L = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lnc8L, !tbaa !4
  %lnc8M = load i64, i64* %R1_Var
  store i64 %lnc8M, i64* %R1_Var
  %lnc8N = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc8O = load i64*, i64** %Sp_Var
  %lnc8P = load i64*, i64** %Hp_Var
  %lnc8Q = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc8N( i64* %Base_Arg, i64* %lnc8O, i64* %lnc8P, i64 %lnc8Q, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_printzustats1_closure_struct = type <{i64, i64}>
@Disk_printzustats1_closure$def = internal global %Disk_printzustats1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_printzustats1_info$def to i64), i64 0}>
@Disk_printzustats1_closure = alias i8* bitcast (%Disk_printzustats1_closure_struct* @Disk_printzustats1_closure$def to i8*)
@Disk_printzustats1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_printzustats1_info$def to i8*)
define ghccc void @Disk_printzustats1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_printzustats1_info$def to i64)),i64 128), i64 8589934597, i64 0, i64 38654705679}>
{
cc92:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnc99 = load i64*, i64** %Sp_Var
  %lnc9a = getelementptr inbounds i64, i64* %lnc99, i32 -1
  %lnc9b = ptrtoint i64* %lnc9a to i64
  %lnc9c = icmp ult i64 %lnc9b, %SpLim_Arg
  br i1 %lnc9c, label %cc93, label %cc94
cc94:
  %lnc9e = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cc8Z_info$def to i64
  %lnc9d = load i64*, i64** %Sp_Var
  %lnc9f = getelementptr inbounds i64, i64* %lnc9d, i32 -1
  store i64 %lnc9e, i64* %lnc9f, !tbaa !1
  %lnc9g = load i64, i64* %R2_Var
  store i64 %lnc9g, i64* %R1_Var
  %lnc9h = load i64*, i64** %Sp_Var
  %lnc9i = getelementptr inbounds i64, i64* %lnc9h, i32 -1
  %lnc9j = ptrtoint i64* %lnc9i to i64
  %lnc9k = inttoptr i64 %lnc9j to i64*
  store i64* %lnc9k, i64** %Sp_Var
  %lnc9l = load i64, i64* %R1_Var
  %lnc9m = and i64 %lnc9l, 7
  %lnc9n = icmp ne i64 %lnc9m, 0
  br i1 %lnc9n, label %uc98, label %cc90
cc90:
  %lnc9p = load i64, i64* %R1_Var
  %lnc9q = inttoptr i64 %lnc9p to i64*
  %lnc9r = load i64, i64* %lnc9q, !tbaa !3
  %lnc9s = inttoptr i64 %lnc9r to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc9t = load i64*, i64** %Sp_Var
  %lnc9u = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc9s( i64* %Base_Arg, i64* %lnc9t, i64* %Hp_Arg, i64 %lnc9u, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
uc98:
  %lnc9v = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cc8Z_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc9w = load i64*, i64** %Sp_Var
  %lnc9x = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc9v( i64* %Base_Arg, i64* %lnc9w, i64* %Hp_Arg, i64 %lnc9x, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cc93:
  %lnc9y = load i64, i64* %R2_Var
  store i64 %lnc9y, i64* %R2_Var
  %lnc9z = ptrtoint %Disk_printzustats1_closure_struct* @Disk_printzustats1_closure$def to i64
  store i64 %lnc9z, i64* %R1_Var
  %lnc9A = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lnc9B = bitcast i64* %lnc9A to i64*
  %lnc9C = load i64, i64* %lnc9B, !tbaa !4
  %lnc9D = inttoptr i64 %lnc9C to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc9E = load i64*, i64** %Sp_Var
  %lnc9F = load i64, i64* %R1_Var
  %lnc9G = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc9D( i64* %Base_Arg, i64* %lnc9E, i64* %Hp_Arg, i64 %lnc9F, i64 %lnc9G, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cc8Z_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cc8Z_info$def to i8*)
define internal ghccc void @cc8Z_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cc8Z_info$def to i64)),i64 128), i64 0, i64 4294967328}>
{
cc8Z:
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnc9H = add i64 %R1_Arg, 23
  %lnc9I = inttoptr i64 %lnc9H to i64*
  %lnc9J = load i64, i64* %lnc9I, !tbaa !3
  store i64 %lnc9J, i64* %R4_Var
  %lnc9K = add i64 %R1_Arg, 15
  %lnc9L = inttoptr i64 %lnc9K to i64*
  %lnc9M = load i64, i64* %lnc9L, !tbaa !3
  store i64 %lnc9M, i64* %R3_Var
  %lnc9N = add i64 %R1_Arg, 7
  %lnc9O = inttoptr i64 %lnc9N to i64*
  %lnc9P = load i64, i64* %lnc9O, !tbaa !3
  store i64 %lnc9P, i64* %R2_Var
  %lnc9Q = load i64*, i64** %Sp_Var
  %lnc9R = getelementptr inbounds i64, i64* %lnc9Q, i32 1
  %lnc9S = ptrtoint i64* %lnc9R to i64
  %lnc9T = inttoptr i64 %lnc9S to i64*
  store i64* %lnc9T, i64** %Sp_Var
  %lnc9U = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwprintzustats_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnc9V = load i64*, i64** %Sp_Var
  %lnc9W = load i64, i64* %R2_Var
  %lnc9X = load i64, i64* %R3_Var
  %lnc9Y = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnc9U( i64* %Base_Arg, i64* %lnc9V, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnc9W, i64 %lnc9X, i64 %lnc9Y, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_printzustats_closure_struct = type <{i64, i64}>
@Disk_printzustats_closure$def = internal global %Disk_printzustats_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_printzustats_info$def to i64), i64 0}>
@Disk_printzustats_closure = alias i8* bitcast (%Disk_printzustats_closure_struct* @Disk_printzustats_closure$def to i8*)
@Disk_printzustats_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_printzustats_info$def to i8*)
define ghccc void @Disk_printzustats_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_printzustats_info$def to i64)),i64 152), i64 8589934597, i64 0, i64 4294967311}>
{
cca4:
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnca7 = load i64, i64* %R2_Var
  store i64 %lnca7, i64* %R2_Var
  %lnca8 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_printzustats1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnca9 = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnca8( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnca9, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_clearzuflushzulog2_closure_struct = type <{i64, i64, i64, i64}>
@Disk_clearzuflushzulog2_closure$def = internal global %Disk_clearzuflushzulog2_closure_struct<{i64 ptrtoint (i8* @Disk_FL_static_info to i64), i64 add (i64 ptrtoint (i8* @ghczmprim_GHCziTypes_ZMZN_closure to i64),i64 1), i64 add (i64 ptrtoint (i8* @ghczmprim_GHCziTypes_ZMZN_closure to i64),i64 1), i64 3}>
@Disk_clearzuflushzulog2_closure = alias i8* bitcast (%Disk_clearzuflushzulog2_closure_struct* @Disk_clearzuflushzulog2_closure$def to i8*)
%Disk_clearzuflushzulog1_closure_struct = type <{i64}>
@Disk_clearzuflushzulog1_closure$def = internal global %Disk_clearzuflushzulog1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_clearzuflushzulog1_info$def to i64)}>
@Disk_clearzuflushzulog1_closure = alias i8* bitcast (%Disk_clearzuflushzulog1_closure_struct* @Disk_clearzuflushzulog1_closure$def to i8*)
@Disk_clearzuflushzulog1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_clearzuflushzulog1_info$def to i8*)
define ghccc void @Disk_clearzuflushzulog1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934597, i64 0, i64 15}>
{
ccaA:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncbh = load i64*, i64** %Sp_Var
  %lncbi = getelementptr inbounds i64, i64* %lncbh, i32 -2
  %lncbj = ptrtoint i64* %lncbi to i64
  %lncbk = icmp ult i64 %lncbj, %SpLim_Arg
  br i1 %lncbk, label %ccaB, label %ccaC
ccaC:
  %lncbm = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccax_info$def to i64
  %lncbl = load i64*, i64** %Sp_Var
  %lncbn = getelementptr inbounds i64, i64* %lncbl, i32 -1
  store i64 %lncbm, i64* %lncbn, !tbaa !1
  %lncbo = load i64, i64* %R2_Var
  store i64 %lncbo, i64* %R1_Var
  %lncbp = load i64*, i64** %Sp_Var
  %lncbq = getelementptr inbounds i64, i64* %lncbp, i32 -1
  %lncbr = ptrtoint i64* %lncbq to i64
  %lncbs = inttoptr i64 %lncbr to i64*
  store i64* %lncbs, i64** %Sp_Var
  %lncbt = load i64, i64* %R1_Var
  %lncbu = and i64 %lncbt, 7
  %lncbv = icmp ne i64 %lncbu, 0
  br i1 %lncbv, label %ucbe, label %ccay
ccay:
  %lncbx = load i64, i64* %R1_Var
  %lncby = inttoptr i64 %lncbx to i64*
  %lncbz = load i64, i64* %lncby, !tbaa !3
  %lncbA = inttoptr i64 %lncbz to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncbB = load i64*, i64** %Sp_Var
  %lncbC = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncbA( i64* %Base_Arg, i64* %lncbB, i64* %Hp_Arg, i64 %lncbC, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucbe:
  %lncbD = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccax_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncbE = load i64*, i64** %Sp_Var
  %lncbF = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncbD( i64* %Base_Arg, i64* %lncbE, i64* %Hp_Arg, i64 %lncbF, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccaB:
  %lncbG = load i64, i64* %R2_Var
  store i64 %lncbG, i64* %R2_Var
  %lncbH = ptrtoint %Disk_clearzuflushzulog1_closure_struct* @Disk_clearzuflushzulog1_closure$def to i64
  store i64 %lncbH, i64* %R1_Var
  %lncbI = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lncbJ = bitcast i64* %lncbI to i64*
  %lncbK = load i64, i64* %lncbJ, !tbaa !4
  %lncbL = inttoptr i64 %lncbK to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncbM = load i64*, i64** %Sp_Var
  %lncbN = load i64, i64* %R1_Var
  %lncbO = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncbL( i64* %Base_Arg, i64* %lncbM, i64* %Hp_Arg, i64 %lncbN, i64 %lncbO, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccax_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccax_info$def to i8*)
define internal ghccc void @ccax_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
ccax:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lncbP = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccaF_info$def to i64
  %lncbQ = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lncbP, i64* %lncbQ, !tbaa !1
  %lncbT = load i64, i64* %R1_Var
  %lncbU = add i64 %lncbT, 15
  %lncbV = inttoptr i64 %lncbU to i64*
  %lncbW = load i64, i64* %lncbV, !tbaa !3
  store i64 %lncbW, i64* %R1_Var
  %lncbX = load i64, i64* %R1_Var
  %lncbY = and i64 %lncbX, 7
  %lncbZ = icmp ne i64 %lncbY, 0
  br i1 %lncbZ, label %ucbd, label %ccaH
ccaH:
  %lncc1 = load i64, i64* %R1_Var
  %lncc2 = inttoptr i64 %lncc1 to i64*
  %lncc3 = load i64, i64* %lncc2, !tbaa !3
  %lncc4 = inttoptr i64 %lncc3 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncc5 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncc4( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lncc5, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucbd:
  %lncc6 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccaF_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncc7 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncc6( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lncc7, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccaF_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccaF_info$def to i8*)
define internal ghccc void @ccaF_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
ccaF:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncc8 = load i64, i64* %R1_Var
  %lncc9 = and i64 %lncc8, 7
  switch i64 %lncc9, label %ccaN [i64 1, label %ccaN
i64 2, label %ccb7]
ccaN:
  %lncca = ptrtoint i8* @ghczmprim_GHCziTypes_ZMZN_closure to i64
  %lnccb = add i64 %lncca, 1
  store i64 %lnccb, i64* %R1_Var
  %lnccc = load i64*, i64** %Sp_Var
  %lnccd = getelementptr inbounds i64, i64* %lnccc, i32 1
  %lncce = ptrtoint i64* %lnccd to i64
  %lnccf = inttoptr i64 %lncce to i64*
  store i64* %lnccf, i64** %Sp_Var
  %lnccg = load i64*, i64** %Sp_Var
  %lncch = getelementptr inbounds i64, i64* %lnccg, i32 0
  %lncci = bitcast i64* %lncch to i64*
  %lnccj = load i64, i64* %lncci, !tbaa !1
  %lncck = inttoptr i64 %lnccj to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnccl = load i64*, i64** %Sp_Var
  %lnccm = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncck( i64* %Base_Arg, i64* %lnccl, i64* %Hp_Arg, i64 %lnccm, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccb7:
  %lncco = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccaQ_info$def to i64
  %lnccn = load i64*, i64** %Sp_Var
  %lnccp = getelementptr inbounds i64, i64* %lnccn, i32 0
  store i64 %lncco, i64* %lnccp, !tbaa !1
  %lnccs = load i64, i64* %R1_Var
  %lncct = add i64 %lnccs, 6
  %lnccu = inttoptr i64 %lncct to i64*
  %lnccv = load i64, i64* %lnccu, !tbaa !3
  store i64 %lnccv, i64* %R1_Var
  %lnccw = load i64, i64* %R1_Var
  %lnccx = and i64 %lnccw, 7
  %lnccy = icmp ne i64 %lnccx, 0
  br i1 %lnccy, label %ucbf, label %ccaR
ccaR:
  %lnccA = load i64, i64* %R1_Var
  %lnccB = inttoptr i64 %lnccA to i64*
  %lnccC = load i64, i64* %lnccB, !tbaa !3
  %lnccD = inttoptr i64 %lnccC to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnccE = load i64*, i64** %Sp_Var
  %lnccF = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnccD( i64* %Base_Arg, i64* %lnccE, i64* %Hp_Arg, i64 %lnccF, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucbf:
  %lnccG = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccaQ_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnccH = load i64*, i64** %Sp_Var
  %lnccI = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnccG( i64* %Base_Arg, i64* %lnccH, i64* %Hp_Arg, i64 %lnccI, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccaQ_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccaQ_info$def to i8*)
define internal ghccc void @ccaQ_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
ccaQ:
  %lsb3W = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnccK = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccaV_info$def to i64
  %lnccJ = load i64*, i64** %Sp_Var
  %lnccL = getelementptr inbounds i64, i64* %lnccJ, i32 -1
  store i64 %lnccK, i64* %lnccL, !tbaa !1
  %lnccO = load i64, i64* %R1_Var
  %lnccP = add i64 %lnccO, 7
  %lnccQ = inttoptr i64 %lnccP to i64*
  %lnccR = load i64, i64* %lnccQ, !tbaa !3
  store i64 %lnccR, i64* %lsb3W
  %lnccS = load i64, i64* %lsb3W
  %lnccT = add i64 %lnccS, 8
  %lnccU = inttoptr i64 %lnccT to i64*
  %lnccV = load i64, i64* %lnccU, !tbaa !5
  store i64 %lnccV, i64* %R1_Var
  %lnccX = load i64, i64* %lsb3W
  %lnccW = load i64*, i64** %Sp_Var
  %lnccY = getelementptr inbounds i64, i64* %lnccW, i32 0
  store i64 %lnccX, i64* %lnccY, !tbaa !1
  %lnccZ = load i64*, i64** %Sp_Var
  %lncd0 = getelementptr inbounds i64, i64* %lnccZ, i32 -1
  %lncd1 = ptrtoint i64* %lncd0 to i64
  %lncd2 = inttoptr i64 %lncd1 to i64*
  store i64* %lncd2, i64** %Sp_Var
  %lncd3 = load i64, i64* %R1_Var
  %lncd4 = and i64 %lncd3, 7
  %lncd5 = icmp ne i64 %lncd4, 0
  br i1 %lncd5, label %ucbg, label %ccaW
ccaW:
  %lncd7 = load i64, i64* %R1_Var
  %lncd8 = inttoptr i64 %lncd7 to i64*
  %lncd9 = load i64, i64* %lncd8, !tbaa !3
  %lncda = inttoptr i64 %lncd9 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncdb = load i64*, i64** %Sp_Var
  %lncdc = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncda( i64* %Base_Arg, i64* %lncdb, i64* %Hp_Arg, i64 %lncdc, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucbg:
  %lncdd = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccaV_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncde = load i64*, i64** %Sp_Var
  %lncdf = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncdd( i64* %Base_Arg, i64* %lncde, i64* %Hp_Arg, i64 %lncdf, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccaV_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccaV_info$def to i8*)
define internal ghccc void @ccaV_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 1, i64 32}>
{
ccaV:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb3W = alloca i64, i32 1
  %lsb41 = alloca i64, i32 1
  %lsb42 = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncdg = load i64*, i64** %Hp_Var
  %lncdh = getelementptr inbounds i64, i64* %lncdg, i32 3
  %lncdi = ptrtoint i64* %lncdh to i64
  %lncdj = inttoptr i64 %lncdi to i64*
  store i64* %lncdj, i64** %Hp_Var
  %lncdk = load i64*, i64** %Hp_Var
  %lncdl = ptrtoint i64* %lncdk to i64
  %lncdm = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lncdn = bitcast i64* %lncdm to i64*
  %lncdo = load i64, i64* %lncdn, !tbaa !4
  %lncdp = icmp ugt i64 %lncdl, %lncdo
  br i1 %lncdp, label %ccbb, label %ccba
ccba:
  %lncdq = load i64*, i64** %Sp_Var
  %lncdr = getelementptr inbounds i64, i64* %lncdq, i32 1
  %lncds = bitcast i64* %lncdr to i64*
  %lncdt = load i64, i64* %lncds, !tbaa !1
  store i64 %lncdt, i64* %lsb3W
  %lncdw = load i64, i64* %R1_Var
  %lncdx = add i64 %lncdw, 7
  %lncdy = inttoptr i64 %lncdx to i64*
  %lncdz = load i64, i64* %lncdy, !tbaa !3
  store i64 %lncdz, i64* %lsb41
  %lncdC = load i64, i64* %R1_Var
  %lncdD = add i64 %lncdC, 15
  %lncdE = inttoptr i64 %lncdD to i64*
  %lncdF = load i64, i64* %lncdE, !tbaa !3
  store i64 %lncdF, i64* %lsb42
  %lncdG = load i64, i64* %lsb3W
  %lncdH = add i64 %lncdG, 8
  %lncdI = ptrtoint %Disk_clearzuflushzulog2_closure_struct* @Disk_clearzuflushzulog2_closure$def to i64
  %lncdJ = add i64 %lncdI, 1
  %lncdK = inttoptr i64 %lncdH to i64*
  store i64 %lncdJ, i64* %lncdK, !tbaa !5
  %lncdL = ptrtoint i64* %Base_Arg to i64
  %lncdM = inttoptr i64 %lncdL to i8*
  %lncdN = load i64, i64* %lsb3W
  %lncdO = inttoptr i64 %lncdN to i8*
  %lncdP = bitcast i8* @dirty_MUT_VAR to void (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  call ccc void (i8*, i8*) %lncdP( i8* %lncdM, i8* %lncdO ) nounwind
  %lncdR = ptrtoint i8* @ghczmprim_GHCziTypes_ZC_con_info to i64
  %lncdQ = load i64*, i64** %Hp_Var
  %lncdS = getelementptr inbounds i64, i64* %lncdQ, i32 -2
  store i64 %lncdR, i64* %lncdS, !tbaa !2
  %lncdU = load i64, i64* %lsb41
  %lncdT = load i64*, i64** %Hp_Var
  %lncdV = getelementptr inbounds i64, i64* %lncdT, i32 -1
  store i64 %lncdU, i64* %lncdV, !tbaa !2
  %lncdX = load i64, i64* %lsb42
  %lncdW = load i64*, i64** %Hp_Var
  %lncdY = getelementptr inbounds i64, i64* %lncdW, i32 0
  store i64 %lncdX, i64* %lncdY, !tbaa !2
  %lnce0 = load i64*, i64** %Hp_Var
  %lnce1 = ptrtoint i64* %lnce0 to i64
  %lnce2 = add i64 %lnce1, -14
  store i64 %lnce2, i64* %R1_Var
  %lnce3 = load i64*, i64** %Sp_Var
  %lnce4 = getelementptr inbounds i64, i64* %lnce3, i32 2
  %lnce5 = ptrtoint i64* %lnce4 to i64
  %lnce6 = inttoptr i64 %lnce5 to i64*
  store i64* %lnce6, i64** %Sp_Var
  %lnce7 = load i64*, i64** %Sp_Var
  %lnce8 = getelementptr inbounds i64, i64* %lnce7, i32 0
  %lnce9 = bitcast i64* %lnce8 to i64*
  %lncea = load i64, i64* %lnce9, !tbaa !1
  %lnceb = inttoptr i64 %lncea to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncec = load i64*, i64** %Sp_Var
  %lnced = load i64*, i64** %Hp_Var
  %lncee = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnceb( i64* %Base_Arg, i64* %lncec, i64* %lnced, i64 %lncee, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccbb:
  %lncef = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lncef, !tbaa !4
  %lnceg = load i64, i64* %R1_Var
  store i64 %lnceg, i64* %R1_Var
  %lnceh = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncei = load i64*, i64** %Sp_Var
  %lncej = load i64*, i64** %Hp_Var
  %lncek = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnceh( i64* %Base_Arg, i64* %lncei, i64* %lncej, i64 %lncek, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_clearzuflushzulog_closure_struct = type <{i64}>
@Disk_clearzuflushzulog_closure$def = internal global %Disk_clearzuflushzulog_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_clearzuflushzulog_info$def to i64)}>
@Disk_clearzuflushzulog_closure = alias i8* bitcast (%Disk_clearzuflushzulog_closure_struct* @Disk_clearzuflushzulog_closure$def to i8*)
@Disk_clearzuflushzulog_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_clearzuflushzulog_info$def to i8*)
define ghccc void @Disk_clearzuflushzulog_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934597, i64 0, i64 15}>
{
cceq:
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncet = load i64, i64* %R2_Var
  store i64 %lncet, i64* %R2_Var
  %lnceu = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_clearzuflushzulog1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncev = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnceu( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lncev, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_getzuflushzulog1_closure_struct = type <{i64}>
@Disk_getzuflushzulog1_closure$def = internal global %Disk_getzuflushzulog1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_getzuflushzulog1_info$def to i64)}>
@Disk_getzuflushzulog1_closure = alias i8* bitcast (%Disk_getzuflushzulog1_closure_struct* @Disk_getzuflushzulog1_closure$def to i8*)
@Disk_getzuflushzulog1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_getzuflushzulog1_info$def to i8*)
define ghccc void @Disk_getzuflushzulog1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934597, i64 0, i64 15}>
{
cceS:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncfu = load i64*, i64** %Sp_Var
  %lncfv = getelementptr inbounds i64, i64* %lncfu, i32 -1
  %lncfw = ptrtoint i64* %lncfv to i64
  %lncfx = icmp ult i64 %lncfw, %SpLim_Arg
  br i1 %lncfx, label %cceT, label %cceU
cceU:
  %lncfz = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cceP_info$def to i64
  %lncfy = load i64*, i64** %Sp_Var
  %lncfA = getelementptr inbounds i64, i64* %lncfy, i32 -1
  store i64 %lncfz, i64* %lncfA, !tbaa !1
  %lncfB = load i64, i64* %R2_Var
  store i64 %lncfB, i64* %R1_Var
  %lncfC = load i64*, i64** %Sp_Var
  %lncfD = getelementptr inbounds i64, i64* %lncfC, i32 -1
  %lncfE = ptrtoint i64* %lncfD to i64
  %lncfF = inttoptr i64 %lncfE to i64*
  store i64* %lncfF, i64** %Sp_Var
  %lncfG = load i64, i64* %R1_Var
  %lncfH = and i64 %lncfG, 7
  %lncfI = icmp ne i64 %lncfH, 0
  br i1 %lncfI, label %ucfr, label %cceQ
cceQ:
  %lncfK = load i64, i64* %R1_Var
  %lncfL = inttoptr i64 %lncfK to i64*
  %lncfM = load i64, i64* %lncfL, !tbaa !3
  %lncfN = inttoptr i64 %lncfM to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncfO = load i64*, i64** %Sp_Var
  %lncfP = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncfN( i64* %Base_Arg, i64* %lncfO, i64* %Hp_Arg, i64 %lncfP, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucfr:
  %lncfQ = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cceP_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncfR = load i64*, i64** %Sp_Var
  %lncfS = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncfQ( i64* %Base_Arg, i64* %lncfR, i64* %Hp_Arg, i64 %lncfS, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cceT:
  %lncfT = load i64, i64* %R2_Var
  store i64 %lncfT, i64* %R2_Var
  %lncfU = ptrtoint %Disk_getzuflushzulog1_closure_struct* @Disk_getzuflushzulog1_closure$def to i64
  store i64 %lncfU, i64* %R1_Var
  %lncfV = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lncfW = bitcast i64* %lncfV to i64*
  %lncfX = load i64, i64* %lncfW, !tbaa !4
  %lncfY = inttoptr i64 %lncfX to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncfZ = load i64*, i64** %Sp_Var
  %lncg0 = load i64, i64* %R1_Var
  %lncg1 = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncfY( i64* %Base_Arg, i64* %lncfZ, i64* %Hp_Arg, i64 %lncg0, i64 %lncg1, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cceP_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cceP_info$def to i8*)
define internal ghccc void @cceP_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
cceP:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lncg2 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cceX_info$def to i64
  %lncg3 = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lncg2, i64* %lncg3, !tbaa !1
  %lncg6 = load i64, i64* %R1_Var
  %lncg7 = add i64 %lncg6, 15
  %lncg8 = inttoptr i64 %lncg7 to i64*
  %lncg9 = load i64, i64* %lncg8, !tbaa !3
  store i64 %lncg9, i64* %R1_Var
  %lncga = load i64, i64* %R1_Var
  %lncgb = and i64 %lncga, 7
  %lncgc = icmp ne i64 %lncgb, 0
  br i1 %lncgc, label %ucfq, label %cceZ
cceZ:
  %lncge = load i64, i64* %R1_Var
  %lncgf = inttoptr i64 %lncge to i64*
  %lncgg = load i64, i64* %lncgf, !tbaa !3
  %lncgh = inttoptr i64 %lncgg to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncgi = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncgh( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lncgi, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucfq:
  %lncgj = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cceX_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncgk = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncgj( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lncgk, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cceX_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cceX_info$def to i8*)
define internal ghccc void @cceX_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
cceX:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncgl = load i64, i64* %R1_Var
  %lncgm = and i64 %lncgl, 7
  switch i64 %lncgm, label %ccf5 [i64 1, label %ccf5
i64 2, label %ccfk]
ccf5:
  %lncgn = ptrtoint i8* @ghczmprim_GHCziTypes_ZMZN_closure to i64
  %lncgo = add i64 %lncgn, 1
  store i64 %lncgo, i64* %R1_Var
  %lncgp = load i64*, i64** %Sp_Var
  %lncgq = getelementptr inbounds i64, i64* %lncgp, i32 1
  %lncgr = ptrtoint i64* %lncgq to i64
  %lncgs = inttoptr i64 %lncgr to i64*
  store i64* %lncgs, i64** %Sp_Var
  %lncgt = load i64*, i64** %Sp_Var
  %lncgu = getelementptr inbounds i64, i64* %lncgt, i32 0
  %lncgv = bitcast i64* %lncgu to i64*
  %lncgw = load i64, i64* %lncgv, !tbaa !1
  %lncgx = inttoptr i64 %lncgw to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncgy = load i64*, i64** %Sp_Var
  %lncgz = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncgx( i64* %Base_Arg, i64* %lncgy, i64* %Hp_Arg, i64 %lncgz, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccfk:
  %lncgB = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccf8_info$def to i64
  %lncgA = load i64*, i64** %Sp_Var
  %lncgC = getelementptr inbounds i64, i64* %lncgA, i32 0
  store i64 %lncgB, i64* %lncgC, !tbaa !1
  %lncgF = load i64, i64* %R1_Var
  %lncgG = add i64 %lncgF, 6
  %lncgH = inttoptr i64 %lncgG to i64*
  %lncgI = load i64, i64* %lncgH, !tbaa !3
  store i64 %lncgI, i64* %R1_Var
  %lncgJ = load i64, i64* %R1_Var
  %lncgK = and i64 %lncgJ, 7
  %lncgL = icmp ne i64 %lncgK, 0
  br i1 %lncgL, label %ucfs, label %ccf9
ccf9:
  %lncgN = load i64, i64* %R1_Var
  %lncgO = inttoptr i64 %lncgN to i64*
  %lncgP = load i64, i64* %lncgO, !tbaa !3
  %lncgQ = inttoptr i64 %lncgP to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncgR = load i64*, i64** %Sp_Var
  %lncgS = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncgQ( i64* %Base_Arg, i64* %lncgR, i64* %Hp_Arg, i64 %lncgS, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucfs:
  %lncgT = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccf8_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncgU = load i64*, i64** %Sp_Var
  %lncgV = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncgT( i64* %Base_Arg, i64* %lncgU, i64* %Hp_Arg, i64 %lncgV, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccf8_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccf8_info$def to i8*)
define internal ghccc void @ccf8_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
ccf8:
  %lsb4i = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lncgY = load i64, i64* %R1_Var
  %lncgZ = add i64 %lncgY, 7
  %lnch0 = inttoptr i64 %lncgZ to i64*
  %lnch1 = load i64, i64* %lnch0, !tbaa !3
  %lnch2 = add i64 %lnch1, 8
  %lnch3 = inttoptr i64 %lnch2 to i64*
  %lnch4 = load i64, i64* %lnch3, !tbaa !5
  store i64 %lnch4, i64* %lsb4i
  %lnch5 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccfd_info$def to i64
  %lnch6 = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lnch5, i64* %lnch6, !tbaa !1
  %lnch7 = load i64, i64* %lsb4i
  store i64 %lnch7, i64* %R1_Var
  %lnch8 = load i64, i64* %R1_Var
  %lnch9 = and i64 %lnch8, 7
  %lncha = icmp ne i64 %lnch9, 0
  br i1 %lncha, label %ucft, label %ccfe
ccfe:
  %lnchc = load i64, i64* %R1_Var
  %lnchd = inttoptr i64 %lnchc to i64*
  %lnche = load i64, i64* %lnchd, !tbaa !3
  %lnchf = inttoptr i64 %lnche to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnchg = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnchf( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnchg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucft:
  %lnchh = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccfd_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnchi = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnchh( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnchi, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccfd_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccfd_info$def to i8*)
define internal ghccc void @ccfd_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
ccfd:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb4k = alloca i64, i32 1
  %lsb4l = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnchj = load i64*, i64** %Hp_Var
  %lnchk = getelementptr inbounds i64, i64* %lnchj, i32 3
  %lnchl = ptrtoint i64* %lnchk to i64
  %lnchm = inttoptr i64 %lnchl to i64*
  store i64* %lnchm, i64** %Hp_Var
  %lnchn = load i64*, i64** %Hp_Var
  %lncho = ptrtoint i64* %lnchn to i64
  %lnchp = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnchq = bitcast i64* %lnchp to i64*
  %lnchr = load i64, i64* %lnchq, !tbaa !4
  %lnchs = icmp ugt i64 %lncho, %lnchr
  br i1 %lnchs, label %ccfo, label %ccfn
ccfn:
  %lnchv = load i64, i64* %R1_Var
  %lnchw = add i64 %lnchv, 7
  %lnchx = inttoptr i64 %lnchw to i64*
  %lnchy = load i64, i64* %lnchx, !tbaa !3
  store i64 %lnchy, i64* %lsb4k
  %lnchB = load i64, i64* %R1_Var
  %lnchC = add i64 %lnchB, 15
  %lnchD = inttoptr i64 %lnchC to i64*
  %lnchE = load i64, i64* %lnchD, !tbaa !3
  store i64 %lnchE, i64* %lsb4l
  %lnchG = ptrtoint i8* @ghczmprim_GHCziTypes_ZC_con_info to i64
  %lnchF = load i64*, i64** %Hp_Var
  %lnchH = getelementptr inbounds i64, i64* %lnchF, i32 -2
  store i64 %lnchG, i64* %lnchH, !tbaa !2
  %lnchJ = load i64, i64* %lsb4k
  %lnchI = load i64*, i64** %Hp_Var
  %lnchK = getelementptr inbounds i64, i64* %lnchI, i32 -1
  store i64 %lnchJ, i64* %lnchK, !tbaa !2
  %lnchM = load i64, i64* %lsb4l
  %lnchL = load i64*, i64** %Hp_Var
  %lnchN = getelementptr inbounds i64, i64* %lnchL, i32 0
  store i64 %lnchM, i64* %lnchN, !tbaa !2
  %lnchP = load i64*, i64** %Hp_Var
  %lnchQ = ptrtoint i64* %lnchP to i64
  %lnchR = add i64 %lnchQ, -14
  store i64 %lnchR, i64* %R1_Var
  %lnchS = load i64*, i64** %Sp_Var
  %lnchT = getelementptr inbounds i64, i64* %lnchS, i32 1
  %lnchU = ptrtoint i64* %lnchT to i64
  %lnchV = inttoptr i64 %lnchU to i64*
  store i64* %lnchV, i64** %Sp_Var
  %lnchW = load i64*, i64** %Sp_Var
  %lnchX = getelementptr inbounds i64, i64* %lnchW, i32 0
  %lnchY = bitcast i64* %lnchX to i64*
  %lnchZ = load i64, i64* %lnchY, !tbaa !1
  %lnci0 = inttoptr i64 %lnchZ to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnci1 = load i64*, i64** %Sp_Var
  %lnci2 = load i64*, i64** %Hp_Var
  %lnci3 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnci0( i64* %Base_Arg, i64* %lnci1, i64* %lnci2, i64 %lnci3, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccfo:
  %lnci4 = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lnci4, !tbaa !4
  %lnci5 = load i64, i64* %R1_Var
  store i64 %lnci5, i64* %R1_Var
  %lnci6 = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnci7 = load i64*, i64** %Sp_Var
  %lnci8 = load i64*, i64** %Hp_Var
  %lnci9 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnci6( i64* %Base_Arg, i64* %lnci7, i64* %lnci8, i64 %lnci9, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_getzuflushzulog_closure_struct = type <{i64}>
@Disk_getzuflushzulog_closure$def = internal global %Disk_getzuflushzulog_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_getzuflushzulog_info$def to i64)}>
@Disk_getzuflushzulog_closure = alias i8* bitcast (%Disk_getzuflushzulog_closure_struct* @Disk_getzuflushzulog_closure$def to i8*)
@Disk_getzuflushzulog_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_getzuflushzulog_info$def to i8*)
define ghccc void @Disk_getzuflushzulog_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934597, i64 0, i64 15}>
{
ccif:
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncii = load i64, i64* %R2_Var
  store i64 %lncii, i64* %R2_Var
  %lncij = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_getzuflushzulog1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncik = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncij( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lncik, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_zdwclosezudisk_closure_struct = type <{i64, i64}>
@Disk_zdwclosezudisk_closure$def = internal global %Disk_zdwclosezudisk_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwclosezudisk_info$def to i64), i64 0}>
@Disk_zdwclosezudisk_closure = alias i8* bitcast (%Disk_zdwclosezudisk_closure_struct* @Disk_zdwclosezudisk_closure$def to i8*)
@Disk_zdwclosezudisk_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwclosezudisk_info$def to i8*)
define ghccc void @Disk_zdwclosezudisk_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwclosezudisk_info$def to i64)),i64 160), i64 17179869203, i64 0, i64 30064771087}>
{
cciC:
  %lsb4o = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 %R4_Arg, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %lsb4u = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnciU = load i64*, i64** %Sp_Var
  %lnciV = getelementptr inbounds i64, i64* %lnciU, i32 -2
  %lnciW = ptrtoint i64* %lnciV to i64
  %lnciX = icmp ult i64 %lnciW, %SpLim_Arg
  br i1 %lnciX, label %cciD, label %cciE
cciE:
  %lnciY = load i64, i64* %R3_Var
  store i64 %lnciY, i64* %lsb4o
  %lnciZ = load i64, i64* %R2_Var
  %lncj0 = bitcast i8* @close to i64 (i64)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lncj1 = call ccc i64 (i64) %lncj0( i64 %lnciZ ) nounwind
  store i64 %lncj1, i64* %lsb4u
  %lncj2 = load i64, i64* %lsb4u
  %lncj3 = trunc i64 %lncj2 to i32
  %lncj4 = sext i32 %lncj3 to i64
  switch i64 %lncj4, label %cciA [i64 -1, label %cciB]
cciA:
  %lncj5 = load i64, i64* %lsb4o
  %lncj6 = add i64 %lncj5, 8
  %lncj7 = inttoptr i64 %lncj6 to i64*
  %lncj8 = load i64, i64* %lncj7, !tbaa !5
  store i64 %lncj8, i64* %R1_Var
  %lncj9 = load i64*, i64** %Sp_Var
  %lncja = getelementptr inbounds i64, i64* %lncj9, i32 0
  %lncjb = bitcast i64* %lncja to i64*
  %lncjc = load i64, i64* %lncjb, !tbaa !1
  %lncjd = inttoptr i64 %lncjc to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncje = load i64*, i64** %Sp_Var
  %lncjf = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncjd( i64* %Base_Arg, i64* %lncje, i64* %Hp_Arg, i64 %lncjf, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cciB:
  %lncjh = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cciP_info$def to i64
  %lncjg = load i64*, i64** %Sp_Var
  %lncji = getelementptr inbounds i64, i64* %lncjg, i32 -2
  store i64 %lncjh, i64* %lncji, !tbaa !1
  %lncjj = ptrtoint i8* @unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_closeFd2_closure to i64
  store i64 %lncjj, i64* %R2_Var
  %lncjl = load i64, i64* %lsb4o
  %lncjk = load i64*, i64** %Sp_Var
  %lncjm = getelementptr inbounds i64, i64* %lncjk, i32 -1
  store i64 %lncjl, i64* %lncjm, !tbaa !1
  %lncjn = load i64*, i64** %Sp_Var
  %lncjo = getelementptr inbounds i64, i64* %lncjn, i32 -2
  %lncjp = ptrtoint i64* %lncjo to i64
  %lncjq = inttoptr i64 %lncjp to i64*
  store i64* %lncjq, i64** %Sp_Var
  %lncjr = bitcast i8* @base_ForeignziCziError_throwErrno1_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncjs = load i64*, i64** %Sp_Var
  %lncjt = load i64, i64* %R1_Var
  %lncju = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncjr( i64* %Base_Arg, i64* %lncjs, i64* %Hp_Arg, i64 %lncjt, i64 %lncju, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cciD:
  %lncjv = load i64, i64* %R4_Var
  store i64 %lncjv, i64* %R4_Var
  %lncjw = load i64, i64* %R3_Var
  store i64 %lncjw, i64* %R3_Var
  %lncjx = load i64, i64* %R2_Var
  store i64 %lncjx, i64* %R2_Var
  %lncjy = ptrtoint %Disk_zdwclosezudisk_closure_struct* @Disk_zdwclosezudisk_closure$def to i64
  store i64 %lncjy, i64* %R1_Var
  %lncjz = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lncjA = bitcast i64* %lncjz to i64*
  %lncjB = load i64, i64* %lncjA, !tbaa !4
  %lncjC = inttoptr i64 %lncjB to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncjD = load i64*, i64** %Sp_Var
  %lncjE = load i64, i64* %R1_Var
  %lncjF = load i64, i64* %R2_Var
  %lncjG = load i64, i64* %R3_Var
  %lncjH = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncjC( i64* %Base_Arg, i64* %lncjD, i64* %Hp_Arg, i64 %lncjE, i64 %lncjF, i64 %lncjG, i64 %lncjH, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cciP_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cciP_info$def to i8*)
define internal ghccc void @cciP_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 1, i64 32}>
{
cciP:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncjI = load i64*, i64** %Sp_Var
  %lncjJ = getelementptr inbounds i64, i64* %lncjI, i32 1
  %lncjK = bitcast i64* %lncjJ to i64*
  %lncjL = load i64, i64* %lncjK, !tbaa !1
  %lncjM = add i64 %lncjL, 8
  %lncjN = inttoptr i64 %lncjM to i64*
  %lncjO = load i64, i64* %lncjN, !tbaa !5
  store i64 %lncjO, i64* %R1_Var
  %lncjP = load i64*, i64** %Sp_Var
  %lncjQ = getelementptr inbounds i64, i64* %lncjP, i32 2
  %lncjR = ptrtoint i64* %lncjQ to i64
  %lncjS = inttoptr i64 %lncjR to i64*
  store i64* %lncjS, i64** %Sp_Var
  %lncjT = load i64*, i64** %Sp_Var
  %lncjU = getelementptr inbounds i64, i64* %lncjT, i32 0
  %lncjV = bitcast i64* %lncjU to i64*
  %lncjW = load i64, i64* %lncjV, !tbaa !1
  %lncjX = inttoptr i64 %lncjW to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncjY = load i64*, i64** %Sp_Var
  %lncjZ = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncjX( i64* %Base_Arg, i64* %lncjY, i64* %Hp_Arg, i64 %lncjZ, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_closezudisk1_closure_struct = type <{i64, i64}>
@Disk_closezudisk1_closure$def = internal global %Disk_closezudisk1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_closezudisk1_info$def to i64), i64 0}>
@Disk_closezudisk1_closure = alias i8* bitcast (%Disk_closezudisk1_closure_struct* @Disk_closezudisk1_closure$def to i8*)
@Disk_closezudisk1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_closezudisk1_info$def to i8*)
define ghccc void @Disk_closezudisk1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_closezudisk1_info$def to i64)),i64 160), i64 8589934597, i64 0, i64 38654705679}>
{
cckb:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncki = load i64*, i64** %Sp_Var
  %lnckj = getelementptr inbounds i64, i64* %lncki, i32 -1
  %lnckk = ptrtoint i64* %lnckj to i64
  %lnckl = icmp ult i64 %lnckk, %SpLim_Arg
  br i1 %lnckl, label %cckc, label %cckd
cckd:
  %lnckn = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cck8_info$def to i64
  %lnckm = load i64*, i64** %Sp_Var
  %lncko = getelementptr inbounds i64, i64* %lnckm, i32 -1
  store i64 %lnckn, i64* %lncko, !tbaa !1
  %lnckp = load i64, i64* %R2_Var
  store i64 %lnckp, i64* %R1_Var
  %lnckq = load i64*, i64** %Sp_Var
  %lnckr = getelementptr inbounds i64, i64* %lnckq, i32 -1
  %lncks = ptrtoint i64* %lnckr to i64
  %lnckt = inttoptr i64 %lncks to i64*
  store i64* %lnckt, i64** %Sp_Var
  %lncku = load i64, i64* %R1_Var
  %lnckv = and i64 %lncku, 7
  %lnckw = icmp ne i64 %lnckv, 0
  br i1 %lnckw, label %uckh, label %cck9
cck9:
  %lncky = load i64, i64* %R1_Var
  %lnckz = inttoptr i64 %lncky to i64*
  %lnckA = load i64, i64* %lnckz, !tbaa !3
  %lnckB = inttoptr i64 %lnckA to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnckC = load i64*, i64** %Sp_Var
  %lnckD = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnckB( i64* %Base_Arg, i64* %lnckC, i64* %Hp_Arg, i64 %lnckD, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
uckh:
  %lnckE = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cck8_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnckF = load i64*, i64** %Sp_Var
  %lnckG = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnckE( i64* %Base_Arg, i64* %lnckF, i64* %Hp_Arg, i64 %lnckG, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cckc:
  %lnckH = load i64, i64* %R2_Var
  store i64 %lnckH, i64* %R2_Var
  %lnckI = ptrtoint %Disk_closezudisk1_closure_struct* @Disk_closezudisk1_closure$def to i64
  store i64 %lnckI, i64* %R1_Var
  %lnckJ = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lnckK = bitcast i64* %lnckJ to i64*
  %lnckL = load i64, i64* %lnckK, !tbaa !4
  %lnckM = inttoptr i64 %lnckL to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnckN = load i64*, i64** %Sp_Var
  %lnckO = load i64, i64* %R1_Var
  %lnckP = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnckM( i64* %Base_Arg, i64* %lnckN, i64* %Hp_Arg, i64 %lnckO, i64 %lnckP, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cck8_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cck8_info$def to i8*)
define internal ghccc void @cck8_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cck8_info$def to i64)),i64 160), i64 0, i64 4294967328}>
{
cck8:
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnckQ = add i64 %R1_Arg, 23
  %lnckR = inttoptr i64 %lnckQ to i64*
  %lnckS = load i64, i64* %lnckR, !tbaa !3
  store i64 %lnckS, i64* %R4_Var
  %lnckT = add i64 %R1_Arg, 7
  %lnckU = inttoptr i64 %lnckT to i64*
  %lnckV = load i64, i64* %lnckU, !tbaa !3
  store i64 %lnckV, i64* %R3_Var
  %lnckW = add i64 %R1_Arg, 31
  %lnckX = inttoptr i64 %lnckW to i64*
  %lnckY = load i64, i64* %lnckX, !tbaa !3
  store i64 %lnckY, i64* %R2_Var
  %lnckZ = load i64*, i64** %Sp_Var
  %lncl0 = getelementptr inbounds i64, i64* %lnckZ, i32 1
  %lncl1 = ptrtoint i64* %lncl0 to i64
  %lncl2 = inttoptr i64 %lncl1 to i64*
  store i64* %lncl2, i64** %Sp_Var
  %lncl3 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwclosezudisk_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncl4 = load i64*, i64** %Sp_Var
  %lncl5 = load i64, i64* %R2_Var
  %lncl6 = load i64, i64* %R3_Var
  %lncl7 = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncl3( i64* %Base_Arg, i64* %lncl4, i64* %Hp_Arg, i64 %R1_Arg, i64 %lncl5, i64 %lncl6, i64 %lncl7, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_closezudisk_closure_struct = type <{i64, i64}>
@Disk_closezudisk_closure$def = internal global %Disk_closezudisk_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_closezudisk_info$def to i64), i64 0}>
@Disk_closezudisk_closure = alias i8* bitcast (%Disk_closezudisk_closure_struct* @Disk_closezudisk_closure$def to i8*)
@Disk_closezudisk_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_closezudisk_info$def to i8*)
define ghccc void @Disk_closezudisk_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_closezudisk_info$def to i64)),i64 184), i64 8589934597, i64 0, i64 4294967311}>
{
ccld:
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnclg = load i64, i64* %R2_Var
  store i64 %lnclg, i64* %R2_Var
  %lnclh = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_closezudisk1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncli = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnclh( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lncli, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_zdwsetzunblockszudisk_closure_struct = type <{i64, i64}>
@Disk_zdwsetzunblockszudisk_closure$def = internal global %Disk_zdwsetzunblockszudisk_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwsetzunblockszudisk_info$def to i64), i64 0}>
@Disk_zdwsetzunblockszudisk_closure = alias i8* bitcast (%Disk_zdwsetzunblockszudisk_closure_struct* @Disk_zdwsetzunblockszudisk_closure$def to i8*)
@Disk_zdwsetzunblockszudisk_slow = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwsetzunblockszudisk_slow$def to i8*)
define ghccc void @Disk_zdwsetzunblockszudisk_slow$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind
{
cclt:
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnclY = load i64*, i64** %Sp_Var
  %lnclZ = getelementptr inbounds i64, i64* %lnclY, i32 3
  %lncm0 = bitcast i64* %lnclZ to i64*
  %lncm1 = load i64, i64* %lncm0, !tbaa !1
  store i64 %lncm1, i64* %R5_Var
  %lncm2 = load i64*, i64** %Sp_Var
  %lncm3 = getelementptr inbounds i64, i64* %lncm2, i32 2
  %lncm4 = bitcast i64* %lncm3 to i64*
  %lncm5 = load i64, i64* %lncm4, !tbaa !1
  store i64 %lncm5, i64* %R4_Var
  %lncm6 = load i64*, i64** %Sp_Var
  %lncm7 = getelementptr inbounds i64, i64* %lncm6, i32 1
  %lncm8 = bitcast i64* %lncm7 to i64*
  %lncm9 = load i64, i64* %lncm8, !tbaa !1
  store i64 %lncm9, i64* %R3_Var
  %lncma = load i64*, i64** %Sp_Var
  %lncmb = getelementptr inbounds i64, i64* %lncma, i32 0
  %lncmc = bitcast i64* %lncmb to i64*
  %lncmd = load i64, i64* %lncmc, !tbaa !1
  store i64 %lncmd, i64* %R2_Var
  %lncme = load i64, i64* %R1_Var
  store i64 %lncme, i64* %R1_Var
  %lncmf = load i64*, i64** %Sp_Var
  %lncmg = getelementptr inbounds i64, i64* %lncmf, i32 4
  %lncmh = ptrtoint i64* %lncmg to i64
  %lncmi = inttoptr i64 %lncmh to i64*
  store i64* %lncmi, i64** %Sp_Var
  %lncmj = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwsetzunblockszudisk_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncmk = load i64*, i64** %Sp_Var
  %lncml = load i64, i64* %R1_Var
  %lncmm = load i64, i64* %R2_Var
  %lncmn = load i64, i64* %R3_Var
  %lncmo = load i64, i64* %R4_Var
  %lncmp = load i64, i64* %R5_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncmj( i64* %Base_Arg, i64* %lncmk, i64* %Hp_Arg, i64 %lncml, i64 %lncmm, i64 %lncmn, i64 %lncmo, i64 %lncmp, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@Disk_zdwsetzunblockszudisk_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwsetzunblockszudisk_info$def to i8*)
define ghccc void @Disk_zdwsetzunblockszudisk_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwsetzunblockszudisk_slow$def to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwsetzunblockszudisk_info$def to i64)),i64 0), i64 580, i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwsetzunblockszudisk_info$def to i64)),i64 176), i64 21474836480, i64 0, i64 55834574863}>
{
cclz:
  %lcclF = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 %R4_Arg, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 %R5_Arg, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %lsb4P = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncmq = load i64*, i64** %Sp_Var
  %lncmr = getelementptr inbounds i64, i64* %lncmq, i32 -1
  %lncms = ptrtoint i64* %lncmr to i64
  %lncmt = icmp ult i64 %lncms, %SpLim_Arg
  br i1 %lncmt, label %cclA, label %cclB
cclB:
  %lncmu = load i64, i64* %R5_Var
  %lncmv = shl i64 %lncmu, 12
  store i64 %lncmv, i64* %lcclF
  %lncmw = load i64, i64* %R2_Var
  %lncmx = load i64, i64* %lcclF
  %lncmy = bitcast i8* @__hscore_ftruncate to i64 (i64, i64)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lncmz = call ccc i64 (i64, i64) %lncmy( i64 %lncmw, i64 %lncmx ) nounwind
  store i64 %lncmz, i64* %lsb4P
  %lncmA = load i64, i64* %lsb4P
  %lncmB = trunc i64 %lncmA to i32
  %lncmC = sext i32 %lncmB to i64
  switch i64 %lncmC, label %cclL [i64 -1, label %cclU]
cclL:
  %lncmD = ptrtoint i8* @ghczmprim_GHCziTuple_Z0T_closure to i64
  %lncmE = add i64 %lncmD, 1
  store i64 %lncmE, i64* %R1_Var
  %lncmF = load i64*, i64** %Sp_Var
  %lncmG = getelementptr inbounds i64, i64* %lncmF, i32 0
  %lncmH = bitcast i64* %lncmG to i64*
  %lncmI = load i64, i64* %lncmH, !tbaa !1
  %lncmJ = inttoptr i64 %lncmI to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncmK = load i64*, i64** %Sp_Var
  %lncmL = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncmJ( i64* %Base_Arg, i64* %lncmK, i64* %Hp_Arg, i64 %lncmL, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cclU:
  %lncmN = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cclT_info$def to i64
  %lncmM = load i64*, i64** %Sp_Var
  %lncmO = getelementptr inbounds i64, i64* %lncmM, i32 -1
  store i64 %lncmN, i64* %lncmO, !tbaa !1
  %lncmP = ptrtoint i8* @unixzm2zi7zi2zi0_SystemziPosixziFilesziCommon_setFdSizze2_closure to i64
  store i64 %lncmP, i64* %R2_Var
  %lncmQ = load i64*, i64** %Sp_Var
  %lncmR = getelementptr inbounds i64, i64* %lncmQ, i32 -1
  %lncmS = ptrtoint i64* %lncmR to i64
  %lncmT = inttoptr i64 %lncmS to i64*
  store i64* %lncmT, i64** %Sp_Var
  %lncmU = bitcast i8* @base_ForeignziCziError_throwErrno1_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncmV = load i64*, i64** %Sp_Var
  %lncmW = load i64, i64* %R1_Var
  %lncmX = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncmU( i64* %Base_Arg, i64* %lncmV, i64* %Hp_Arg, i64 %lncmW, i64 %lncmX, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cclA:
  %lncmY = ptrtoint %Disk_zdwsetzunblockszudisk_closure_struct* @Disk_zdwsetzunblockszudisk_closure$def to i64
  store i64 %lncmY, i64* %R1_Var
  %lncn0 = load i64, i64* %R2_Var
  %lncmZ = load i64*, i64** %Sp_Var
  %lncn1 = getelementptr inbounds i64, i64* %lncmZ, i32 -4
  store i64 %lncn0, i64* %lncn1, !tbaa !1
  %lncn3 = load i64, i64* %R3_Var
  %lncn2 = load i64*, i64** %Sp_Var
  %lncn4 = getelementptr inbounds i64, i64* %lncn2, i32 -3
  store i64 %lncn3, i64* %lncn4, !tbaa !1
  %lncn6 = load i64, i64* %R4_Var
  %lncn5 = load i64*, i64** %Sp_Var
  %lncn7 = getelementptr inbounds i64, i64* %lncn5, i32 -2
  store i64 %lncn6, i64* %lncn7, !tbaa !1
  %lncn9 = load i64, i64* %R5_Var
  %lncn8 = load i64*, i64** %Sp_Var
  %lncna = getelementptr inbounds i64, i64* %lncn8, i32 -1
  store i64 %lncn9, i64* %lncna, !tbaa !1
  %lncnb = load i64*, i64** %Sp_Var
  %lncnc = getelementptr inbounds i64, i64* %lncnb, i32 -4
  %lncnd = ptrtoint i64* %lncnc to i64
  %lncne = inttoptr i64 %lncnd to i64*
  store i64* %lncne, i64** %Sp_Var
  %lncnf = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lncng = bitcast i64* %lncnf to i64*
  %lncnh = load i64, i64* %lncng, !tbaa !4
  %lncni = inttoptr i64 %lncnh to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncnj = load i64*, i64** %Sp_Var
  %lncnk = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncni( i64* %Base_Arg, i64* %lncnj, i64* %Hp_Arg, i64 %lncnk, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cclT_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cclT_info$def to i8*)
define internal ghccc void @cclT_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
cclT:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncnl = ptrtoint i8* @ghczmprim_GHCziTuple_Z0T_closure to i64
  %lncnm = add i64 %lncnl, 1
  store i64 %lncnm, i64* %R1_Var
  %lncnn = load i64*, i64** %Sp_Var
  %lncno = getelementptr inbounds i64, i64* %lncnn, i32 1
  %lncnp = ptrtoint i64* %lncno to i64
  %lncnq = inttoptr i64 %lncnp to i64*
  store i64* %lncnq, i64** %Sp_Var
  %lncnr = load i64*, i64** %Sp_Var
  %lncns = getelementptr inbounds i64, i64* %lncnr, i32 0
  %lncnt = bitcast i64* %lncns to i64*
  %lncnu = load i64, i64* %lncnt, !tbaa !1
  %lncnv = inttoptr i64 %lncnu to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncnw = load i64*, i64** %Sp_Var
  %lncnx = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncnv( i64* %Base_Arg, i64* %lncnw, i64* %Hp_Arg, i64 %lncnx, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_setzunblockszudisk1_closure_struct = type <{i64, i64}>
@Disk_setzunblockszudisk1_closure$def = internal global %Disk_setzunblockszudisk1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_setzunblockszudisk1_info$def to i64), i64 0}>
@Disk_setzunblockszudisk1_closure = alias i8* bitcast (%Disk_setzunblockszudisk1_closure_struct* @Disk_setzunblockszudisk1_closure$def to i8*)
@Disk_setzunblockszudisk1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_setzunblockszudisk1_info$def to i8*)
define ghccc void @Disk_setzunblockszudisk1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_setzunblockszudisk1_info$def to i64)),i64 192), i64 12884901903, i64 0, i64 21474836495}>
{
ccnM:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lnco0 = load i64*, i64** %Sp_Var
  %lnco1 = getelementptr inbounds i64, i64* %lnco0, i32 -4
  %lnco2 = ptrtoint i64* %lnco1 to i64
  %lnco3 = icmp ult i64 %lnco2, %SpLim_Arg
  br i1 %lnco3, label %ccnQ, label %ccnR
ccnR:
  %lnco5 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccnJ_info$def to i64
  %lnco4 = load i64*, i64** %Sp_Var
  %lnco6 = getelementptr inbounds i64, i64* %lnco4, i32 -2
  store i64 %lnco5, i64* %lnco6, !tbaa !1
  %lnco7 = load i64, i64* %R2_Var
  store i64 %lnco7, i64* %R1_Var
  %lnco9 = load i64, i64* %R3_Var
  %lnco8 = load i64*, i64** %Sp_Var
  %lncoa = getelementptr inbounds i64, i64* %lnco8, i32 -1
  store i64 %lnco9, i64* %lncoa, !tbaa !1
  %lncob = load i64*, i64** %Sp_Var
  %lncoc = getelementptr inbounds i64, i64* %lncob, i32 -2
  %lncod = ptrtoint i64* %lncoc to i64
  %lncoe = inttoptr i64 %lncod to i64*
  store i64* %lncoe, i64** %Sp_Var
  %lncof = load i64, i64* %R1_Var
  %lncog = and i64 %lncof, 7
  %lncoh = icmp ne i64 %lncog, 0
  br i1 %lncoh, label %ucnZ, label %ccnK
ccnK:
  %lncoj = load i64, i64* %R1_Var
  %lncok = inttoptr i64 %lncoj to i64*
  %lncol = load i64, i64* %lncok, !tbaa !3
  %lncom = inttoptr i64 %lncol to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncon = load i64*, i64** %Sp_Var
  %lncoo = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncom( i64* %Base_Arg, i64* %lncon, i64* %Hp_Arg, i64 %lncoo, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucnZ:
  %lncop = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccnJ_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncoq = load i64*, i64** %Sp_Var
  %lncor = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncop( i64* %Base_Arg, i64* %lncoq, i64* %Hp_Arg, i64 %lncor, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccnQ:
  %lncos = load i64, i64* %R3_Var
  store i64 %lncos, i64* %R3_Var
  %lncot = load i64, i64* %R2_Var
  store i64 %lncot, i64* %R2_Var
  %lncou = ptrtoint %Disk_setzunblockszudisk1_closure_struct* @Disk_setzunblockszudisk1_closure$def to i64
  store i64 %lncou, i64* %R1_Var
  %lncov = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lncow = bitcast i64* %lncov to i64*
  %lncox = load i64, i64* %lncow, !tbaa !4
  %lncoy = inttoptr i64 %lncox to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncoz = load i64*, i64** %Sp_Var
  %lncoA = load i64, i64* %R1_Var
  %lncoB = load i64, i64* %R2_Var
  %lncoC = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncoy( i64* %Base_Arg, i64* %lncoz, i64* %Hp_Arg, i64 %lncoA, i64 %lncoB, i64 %lncoC, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccnJ_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccnJ_info$def to i8*)
define internal ghccc void @ccnJ_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccnJ_info$def to i64)),i64 192), i64 1, i64 4294967328}>
{
ccnJ:
  %lsb4Z = alloca i64, i32 1
  %lsb51 = alloca i64, i32 1
  %lsb4Y = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncoE = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccnP_info$def to i64
  %lncoD = load i64*, i64** %Sp_Var
  %lncoF = getelementptr inbounds i64, i64* %lncoD, i32 -2
  store i64 %lncoE, i64* %lncoF, !tbaa !1
  %lncoI = load i64, i64* %R1_Var
  %lncoJ = add i64 %lncoI, 7
  %lncoK = inttoptr i64 %lncoJ to i64*
  %lncoL = load i64, i64* %lncoK, !tbaa !3
  store i64 %lncoL, i64* %lsb4Z
  %lncoO = load i64, i64* %R1_Var
  %lncoP = add i64 %lncoO, 23
  %lncoQ = inttoptr i64 %lncoP to i64*
  %lncoR = load i64, i64* %lncoQ, !tbaa !3
  store i64 %lncoR, i64* %lsb51
  %lncoU = load i64, i64* %R1_Var
  %lncoV = add i64 %lncoU, 31
  %lncoW = inttoptr i64 %lncoV to i64*
  %lncoX = load i64, i64* %lncoW, !tbaa !3
  store i64 %lncoX, i64* %lsb4Y
  %lncoY = load i64*, i64** %Sp_Var
  %lncoZ = getelementptr inbounds i64, i64* %lncoY, i32 1
  %lncp0 = bitcast i64* %lncoZ to i64*
  %lncp1 = load i64, i64* %lncp0, !tbaa !1
  store i64 %lncp1, i64* %R1_Var
  %lncp3 = load i64, i64* %lsb51
  %lncp2 = load i64*, i64** %Sp_Var
  %lncp4 = getelementptr inbounds i64, i64* %lncp2, i32 -1
  store i64 %lncp3, i64* %lncp4, !tbaa !1
  %lncp6 = load i64, i64* %lsb4Z
  %lncp5 = load i64*, i64** %Sp_Var
  %lncp7 = getelementptr inbounds i64, i64* %lncp5, i32 0
  store i64 %lncp6, i64* %lncp7, !tbaa !1
  %lncp9 = load i64, i64* %lsb4Y
  %lncp8 = load i64*, i64** %Sp_Var
  %lncpa = getelementptr inbounds i64, i64* %lncp8, i32 1
  store i64 %lncp9, i64* %lncpa, !tbaa !1
  %lncpb = load i64*, i64** %Sp_Var
  %lncpc = getelementptr inbounds i64, i64* %lncpb, i32 -2
  %lncpd = ptrtoint i64* %lncpc to i64
  %lncpe = inttoptr i64 %lncpd to i64*
  store i64* %lncpe, i64** %Sp_Var
  %lncpf = load i64, i64* %R1_Var
  %lncpg = and i64 %lncpf, 7
  %lncph = icmp ne i64 %lncpg, 0
  br i1 %lncph, label %ucnY, label %ccnT
ccnT:
  %lncpj = load i64, i64* %R1_Var
  %lncpk = inttoptr i64 %lncpj to i64*
  %lncpl = load i64, i64* %lncpk, !tbaa !3
  %lncpm = inttoptr i64 %lncpl to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncpn = load i64*, i64** %Sp_Var
  %lncpo = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncpm( i64* %Base_Arg, i64* %lncpn, i64* %Hp_Arg, i64 %lncpo, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucnY:
  %lncpp = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccnP_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncpq = load i64*, i64** %Sp_Var
  %lncpr = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncpp( i64* %Base_Arg, i64* %lncpq, i64* %Hp_Arg, i64 %lncpr, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccnP_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccnP_info$def to i8*)
define internal ghccc void @ccnP_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccnP_info$def to i64)),i64 192), i64 259, i64 4294967328}>
{
ccnP:
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncps = add i64 %R1_Arg, 7
  %lncpt = inttoptr i64 %lncps to i64*
  %lncpu = load i64, i64* %lncpt, !tbaa !3
  store i64 %lncpu, i64* %R5_Var
  %lncpv = load i64*, i64** %Sp_Var
  %lncpw = getelementptr inbounds i64, i64* %lncpv, i32 1
  %lncpx = bitcast i64* %lncpw to i64*
  %lncpy = load i64, i64* %lncpx, !tbaa !1
  store i64 %lncpy, i64* %R4_Var
  %lncpz = load i64*, i64** %Sp_Var
  %lncpA = getelementptr inbounds i64, i64* %lncpz, i32 2
  %lncpB = bitcast i64* %lncpA to i64*
  %lncpC = load i64, i64* %lncpB, !tbaa !1
  store i64 %lncpC, i64* %R3_Var
  %lncpD = load i64*, i64** %Sp_Var
  %lncpE = getelementptr inbounds i64, i64* %lncpD, i32 3
  %lncpF = bitcast i64* %lncpE to i64*
  %lncpG = load i64, i64* %lncpF, !tbaa !1
  store i64 %lncpG, i64* %R2_Var
  %lncpH = load i64*, i64** %Sp_Var
  %lncpI = getelementptr inbounds i64, i64* %lncpH, i32 4
  %lncpJ = ptrtoint i64* %lncpI to i64
  %lncpK = inttoptr i64 %lncpJ to i64*
  store i64* %lncpK, i64** %Sp_Var
  %lncpL = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwsetzunblockszudisk_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncpM = load i64*, i64** %Sp_Var
  %lncpN = load i64, i64* %R2_Var
  %lncpO = load i64, i64* %R3_Var
  %lncpP = load i64, i64* %R4_Var
  %lncpQ = load i64, i64* %R5_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncpL( i64* %Base_Arg, i64* %lncpM, i64* %Hp_Arg, i64 %R1_Arg, i64 %lncpN, i64 %lncpO, i64 %lncpP, i64 %lncpQ, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_setzunblockszudisk_closure_struct = type <{i64, i64}>
@Disk_setzunblockszudisk_closure$def = internal global %Disk_setzunblockszudisk_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_setzunblockszudisk_info$def to i64), i64 0}>
@Disk_setzunblockszudisk_closure = alias i8* bitcast (%Disk_setzunblockszudisk_closure_struct* @Disk_setzunblockszudisk_closure$def to i8*)
@Disk_setzunblockszudisk_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_setzunblockszudisk_info$def to i8*)
define ghccc void @Disk_setzunblockszudisk_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_setzunblockszudisk_info$def to i64)),i64 208), i64 12884901903, i64 0, i64 4294967311}>
{
ccpW:
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncpZ = load i64, i64* %R3_Var
  store i64 %lncpZ, i64* %R3_Var
  %lncq0 = load i64, i64* %R2_Var
  store i64 %lncq0, i64* %R2_Var
  %lncq1 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_setzunblockszudisk1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncq2 = load i64, i64* %R2_Var
  %lncq3 = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncq1( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lncq2, i64 %lncq3, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_initzudisk5_closure_struct = type <{i64, i64}>
@Disk_initzudisk5_closure$def = internal global %Disk_initzudisk5_closure_struct<{i64 ptrtoint (i8* @base_GHCziWord_W32zh_static_info to i64), i64 438}>
@Disk_initzudisk5_closure = alias i8* bitcast (%Disk_initzudisk5_closure_struct* @Disk_initzudisk5_closure$def to i8*)
%Disk_initzudisk4_closure_struct = type <{i64, i64, i64}>
@Disk_initzudisk4_closure$def = internal global %Disk_initzudisk4_closure_struct<{i64 ptrtoint (i8* @base_GHCziBase_Just_static_info to i64), i64 add (i64 ptrtoint (%Disk_initzudisk5_closure_struct* @Disk_initzudisk5_closure$def to i64),i64 1), i64 3}>
@Disk_initzudisk4_closure = alias i8* bitcast (%Disk_initzudisk4_closure_struct* @Disk_initzudisk4_closure$def to i8*)
%Disk_initzudisk3_closure_struct = type <{i64, i64}>
@Disk_initzudisk3_closure$def = internal global %Disk_initzudisk3_closure_struct<{i64 ptrtoint (i8* @integerzmgmp_GHCziIntegerziType_Szh_static_info to i64), i64 0}>
@Disk_initzudisk3_closure = alias i8* bitcast (%Disk_initzudisk3_closure_struct* @Disk_initzudisk3_closure$def to i8*)
%Disk_initzudisk2_closure_struct = type <{i64, i64, i64, i64}>
@Disk_initzudisk2_closure$def = internal global %Disk_initzudisk2_closure_struct<{i64 ptrtoint (i8* @Disk_VS_static_info to i64), i64 add (i64 ptrtoint (i8* @containerszm0zi5zi7zi1_DataziMapziBase_Tip_closure to i64),i64 2), i64 add (i64 ptrtoint (%Disk_initzudisk3_closure_struct* @Disk_initzudisk3_closure$def to i64),i64 1), i64 3}>
@Disk_initzudisk2_closure = alias i8* bitcast (%Disk_initzudisk2_closure_struct* @Disk_initzudisk2_closure$def to i8*)
%Disk_initzudiskzucrashlog1_closure_struct = type <{i64, i64}>
@Disk_initzudiskzucrashlog1_closure$def = internal global %Disk_initzudiskzucrashlog1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_initzudiskzucrashlog1_info$def to i64), i64 0}>
@Disk_initzudiskzucrashlog1_closure = alias i8* bitcast (%Disk_initzudiskzucrashlog1_closure_struct* @Disk_initzudiskzucrashlog1_closure$def to i8*)
@sb5m_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb5m_info$def to i8*)
define internal ghccc void @sb5m_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 4, i64 16}>
{
ccqA:
  %lsb5b = alloca i64, i32 1
  %lsb5e = alloca i64, i32 1
  %lsb5h = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncqV = load i64*, i64** %Sp_Var
  %lncqW = getelementptr inbounds i64, i64* %lncqV, i32 -6
  %lncqX = ptrtoint i64* %lncqW to i64
  %lncqY = icmp ult i64 %lncqX, %SpLim_Arg
  br i1 %lncqY, label %ccqG, label %ccqH
ccqH:
  %lncr0 = ptrtoint i8* @stg_upd_frame_info to i64
  %lncqZ = load i64*, i64** %Sp_Var
  %lncr1 = getelementptr inbounds i64, i64* %lncqZ, i32 -2
  store i64 %lncr0, i64* %lncr1, !tbaa !1
  %lncr3 = load i64, i64* %R1_Var
  %lncr2 = load i64*, i64** %Sp_Var
  %lncr4 = getelementptr inbounds i64, i64* %lncr2, i32 -1
  store i64 %lncr3, i64* %lncr4, !tbaa !1
  %lncr6 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccqx_info$def to i64
  %lncr5 = load i64*, i64** %Sp_Var
  %lncr7 = getelementptr inbounds i64, i64* %lncr5, i32 -6
  store i64 %lncr6, i64* %lncr7, !tbaa !1
  %lncra = load i64, i64* %R1_Var
  %lncrb = add i64 %lncra, 24
  %lncrc = inttoptr i64 %lncrb to i64*
  %lncrd = load i64, i64* %lncrc, !tbaa !3
  store i64 %lncrd, i64* %lsb5b
  %lncrg = load i64, i64* %R1_Var
  %lncrh = add i64 %lncrg, 32
  %lncri = inttoptr i64 %lncrh to i64*
  %lncrj = load i64, i64* %lncri, !tbaa !3
  store i64 %lncrj, i64* %lsb5e
  %lncrm = load i64, i64* %R1_Var
  %lncrn = add i64 %lncrm, 40
  %lncro = inttoptr i64 %lncrn to i64*
  %lncrp = load i64, i64* %lncro, !tbaa !3
  store i64 %lncrp, i64* %lsb5h
  %lncrs = load i64, i64* %R1_Var
  %lncrt = add i64 %lncrs, 16
  %lncru = inttoptr i64 %lncrt to i64*
  %lncrv = load i64, i64* %lncru, !tbaa !3
  store i64 %lncrv, i64* %R1_Var
  %lncrx = load i64, i64* %lsb5b
  %lncrw = load i64*, i64** %Sp_Var
  %lncry = getelementptr inbounds i64, i64* %lncrw, i32 -5
  store i64 %lncrx, i64* %lncry, !tbaa !1
  %lncrA = load i64, i64* %lsb5e
  %lncrz = load i64*, i64** %Sp_Var
  %lncrB = getelementptr inbounds i64, i64* %lncrz, i32 -4
  store i64 %lncrA, i64* %lncrB, !tbaa !1
  %lncrD = load i64, i64* %lsb5h
  %lncrC = load i64*, i64** %Sp_Var
  %lncrE = getelementptr inbounds i64, i64* %lncrC, i32 -3
  store i64 %lncrD, i64* %lncrE, !tbaa !1
  %lncrF = load i64*, i64** %Sp_Var
  %lncrG = getelementptr inbounds i64, i64* %lncrF, i32 -6
  %lncrH = ptrtoint i64* %lncrG to i64
  %lncrI = inttoptr i64 %lncrH to i64*
  store i64* %lncrI, i64** %Sp_Var
  %lncrJ = load i64, i64* %R1_Var
  %lncrK = and i64 %lncrJ, 7
  %lncrL = icmp ne i64 %lncrK, 0
  br i1 %lncrL, label %ucqL, label %ccqy
ccqy:
  %lncrN = load i64, i64* %R1_Var
  %lncrO = inttoptr i64 %lncrN to i64*
  %lncrP = load i64, i64* %lncrO, !tbaa !3
  %lncrQ = inttoptr i64 %lncrP to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncrR = load i64*, i64** %Sp_Var
  %lncrS = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncrQ( i64* %Base_Arg, i64* %lncrR, i64* %Hp_Arg, i64 %lncrS, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucqL:
  %lncrT = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccqx_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncrU = load i64*, i64** %Sp_Var
  %lncrV = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncrT( i64* %Base_Arg, i64* %lncrU, i64* %Hp_Arg, i64 %lncrV, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccqG:
  %lncrW = load i64, i64* %R1_Var
  store i64 %lncrW, i64* %R1_Var
  %lncrX = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lncrY = bitcast i64* %lncrX to i64*
  %lncrZ = load i64, i64* %lncrY, !tbaa !4
  %lncs0 = inttoptr i64 %lncrZ to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncs1 = load i64*, i64** %Sp_Var
  %lncs2 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncs0( i64* %Base_Arg, i64* %lncs1, i64* %Hp_Arg, i64 %lncs2, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccqx_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccqx_info$def to i8*)
define internal ghccc void @ccqx_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 3, i64 32}>
{
ccqx:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb5j = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncs3 = load i64*, i64** %Hp_Var
  %lncs4 = getelementptr inbounds i64, i64* %lncs3, i32 9
  %lncs5 = ptrtoint i64* %lncs4 to i64
  %lncs6 = inttoptr i64 %lncs5 to i64*
  store i64* %lncs6, i64** %Hp_Var
  %lncs7 = load i64*, i64** %Hp_Var
  %lncs8 = ptrtoint i64* %lncs7 to i64
  %lncs9 = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lncsa = bitcast i64* %lncs9 to i64*
  %lncsb = load i64, i64* %lncsa, !tbaa !4
  %lncsc = icmp ugt i64 %lncs8, %lncsb
  br i1 %lncsc, label %ccqK, label %ccqJ
ccqJ:
  %lncsf = load i64, i64* %R1_Var
  %lncsg = add i64 %lncsf, 7
  %lncsh = inttoptr i64 %lncsg to i64*
  %lncsi = load i64, i64* %lncsh, !tbaa !3
  store i64 %lncsi, i64* %lsb5j
  %lncsk = ptrtoint i8* @base_GHCziSTRef_STRef_con_info to i64
  %lncsj = load i64*, i64** %Hp_Var
  %lncsl = getelementptr inbounds i64, i64* %lncsj, i32 -8
  store i64 %lncsk, i64* %lncsl, !tbaa !2
  %lncsn = load i64*, i64** %Sp_Var
  %lncso = getelementptr inbounds i64, i64* %lncsn, i32 2
  %lncsp = bitcast i64* %lncso to i64*
  %lncsq = load i64, i64* %lncsp, !tbaa !1
  %lncsm = load i64*, i64** %Hp_Var
  %lncsr = getelementptr inbounds i64, i64* %lncsm, i32 -7
  store i64 %lncsq, i64* %lncsr, !tbaa !2
  %lncst = ptrtoint i8* @base_GHCziBase_Just_con_info to i64
  %lncss = load i64*, i64** %Hp_Var
  %lncsu = getelementptr inbounds i64, i64* %lncss, i32 -6
  store i64 %lncst, i64* %lncsu, !tbaa !2
  %lncsx = load i64*, i64** %Hp_Var
  %lncsy = ptrtoint i64* %lncsx to i64
  %lncsz = add i64 %lncsy, -63
  %lncsv = load i64*, i64** %Hp_Var
  %lncsA = getelementptr inbounds i64, i64* %lncsv, i32 -5
  store i64 %lncsz, i64* %lncsA, !tbaa !2
  %lncsC = ptrtoint i8* @Disk_S_con_info to i64
  %lncsB = load i64*, i64** %Hp_Var
  %lncsD = getelementptr inbounds i64, i64* %lncsB, i32 -4
  store i64 %lncsC, i64* %lncsD, !tbaa !2
  %lncsF = load i64*, i64** %Sp_Var
  %lncsG = getelementptr inbounds i64, i64* %lncsF, i32 1
  %lncsH = bitcast i64* %lncsG to i64*
  %lncsI = load i64, i64* %lncsH, !tbaa !1
  %lncsE = load i64*, i64** %Hp_Var
  %lncsJ = getelementptr inbounds i64, i64* %lncsE, i32 -3
  store i64 %lncsI, i64* %lncsJ, !tbaa !2
  %lncsM = load i64*, i64** %Hp_Var
  %lncsN = ptrtoint i64* %lncsM to i64
  %lncsO = add i64 %lncsN, -46
  %lncsK = load i64*, i64** %Hp_Var
  %lncsP = getelementptr inbounds i64, i64* %lncsK, i32 -2
  store i64 %lncsO, i64* %lncsP, !tbaa !2
  %lncsR = load i64*, i64** %Sp_Var
  %lncsS = getelementptr inbounds i64, i64* %lncsR, i32 3
  %lncsT = bitcast i64* %lncsS to i64*
  %lncsU = load i64, i64* %lncsT, !tbaa !1
  %lncsQ = load i64*, i64** %Hp_Var
  %lncsV = getelementptr inbounds i64, i64* %lncsQ, i32 -1
  store i64 %lncsU, i64* %lncsV, !tbaa !2
  %lncsX = load i64, i64* %lsb5j
  %lncsW = load i64*, i64** %Hp_Var
  %lncsY = getelementptr inbounds i64, i64* %lncsW, i32 0
  store i64 %lncsX, i64* %lncsY, !tbaa !2
  %lnct0 = load i64*, i64** %Hp_Var
  %lnct1 = ptrtoint i64* %lnct0 to i64
  %lnct2 = add i64 %lnct1, -31
  store i64 %lnct2, i64* %R1_Var
  %lnct3 = load i64*, i64** %Sp_Var
  %lnct4 = getelementptr inbounds i64, i64* %lnct3, i32 4
  %lnct5 = ptrtoint i64* %lnct4 to i64
  %lnct6 = inttoptr i64 %lnct5 to i64*
  store i64* %lnct6, i64** %Sp_Var
  %lnct7 = load i64*, i64** %Sp_Var
  %lnct8 = getelementptr inbounds i64, i64* %lnct7, i32 0
  %lnct9 = bitcast i64* %lnct8 to i64*
  %lncta = load i64, i64* %lnct9, !tbaa !1
  %lnctb = inttoptr i64 %lncta to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnctc = load i64*, i64** %Sp_Var
  %lnctd = load i64*, i64** %Hp_Var
  %lncte = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnctb( i64* %Base_Arg, i64* %lnctc, i64* %lnctd, i64 %lncte, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccqK:
  %lnctf = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 72, i64* %lnctf, !tbaa !4
  %lnctg = load i64, i64* %R1_Var
  store i64 %lnctg, i64* %R1_Var
  %lncth = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncti = load i64*, i64** %Sp_Var
  %lnctj = load i64*, i64** %Hp_Var
  %lnctk = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncth( i64* %Base_Arg, i64* %lncti, i64* %lnctj, i64 %lnctk, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@Disk_initzudiskzucrashlog1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_initzudiskzucrashlog1_info$def to i8*)
define ghccc void @Disk_initzudiskzucrashlog1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_initzudiskzucrashlog1_info$def to i64)),i64 216), i64 8589934597, i64 0, i64 12884901903}>
{
ccqM:
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnctl = load i64*, i64** %Sp_Var
  %lnctm = getelementptr inbounds i64, i64* %lnctl, i32 -4
  %lnctn = ptrtoint i64* %lnctm to i64
  %lncto = icmp ult i64 %lnctn, %SpLim_Arg
  br i1 %lncto, label %ccqN, label %ccqO
ccqO:
  %lnctq = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccql_info$def to i64
  %lnctp = load i64*, i64** %Sp_Var
  %lnctr = getelementptr inbounds i64, i64* %lnctp, i32 -1
  store i64 %lnctq, i64* %lnctr, !tbaa !1
  %lncts = ptrtoint i8* @unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_defaultFileFlags_closure to i64
  store i64 %lncts, i64* %R5_Var
  %lnctt = ptrtoint %Disk_initzudisk4_closure_struct* @Disk_initzudisk4_closure$def to i64
  %lnctu = add i64 %lnctt, 2
  store i64 %lnctu, i64* %R4_Var
  %lnctv = ptrtoint i8* @unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_ReadWrite_closure to i64
  %lnctw = add i64 %lnctv, 3
  store i64 %lnctw, i64* %R3_Var
  %lnctx = load i64, i64* %R2_Var
  store i64 %lnctx, i64* %R2_Var
  %lncty = load i64*, i64** %Sp_Var
  %lnctz = getelementptr inbounds i64, i64* %lncty, i32 -1
  %lnctA = ptrtoint i64* %lnctz to i64
  %lnctB = inttoptr i64 %lnctA to i64*
  store i64* %lnctB, i64** %Sp_Var
  %lnctC = bitcast i8* @unixzm2zi7zi2zi0_SystemziPosixziIO_createFile3_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnctD = load i64*, i64** %Sp_Var
  %lnctE = load i64, i64* %R1_Var
  %lnctF = load i64, i64* %R2_Var
  %lnctG = load i64, i64* %R3_Var
  %lnctH = load i64, i64* %R4_Var
  %lnctI = load i64, i64* %R5_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnctC( i64* %Base_Arg, i64* %lnctD, i64* %Hp_Arg, i64 %lnctE, i64 %lnctF, i64 %lnctG, i64 %lnctH, i64 %lnctI, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccqN:
  %lnctJ = load i64, i64* %R2_Var
  store i64 %lnctJ, i64* %R2_Var
  %lnctK = ptrtoint %Disk_initzudiskzucrashlog1_closure_struct* @Disk_initzudiskzucrashlog1_closure$def to i64
  store i64 %lnctK, i64* %R1_Var
  %lnctL = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lnctM = bitcast i64* %lnctL to i64*
  %lnctN = load i64, i64* %lnctM, !tbaa !4
  %lnctO = inttoptr i64 %lnctN to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnctP = load i64*, i64** %Sp_Var
  %lnctQ = load i64, i64* %R1_Var
  %lnctR = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnctO( i64* %Base_Arg, i64* %lnctP, i64* %Hp_Arg, i64 %lnctQ, i64 %lnctR, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccql_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccql_info$def to i8*)
define internal ghccc void @ccql_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
ccql:
  %lsb58 = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnctT = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccqn_info$def to i64
  %lnctS = load i64*, i64** %Sp_Var
  %lnctU = getelementptr inbounds i64, i64* %lnctS, i32 -1
  store i64 %lnctT, i64* %lnctU, !tbaa !1
  %lnctV = load i64, i64* %R1_Var
  store i64 %lnctV, i64* %lsb58
  %lnctW = ptrtoint %Disk_clearzustats2_closure_struct* @Disk_clearzustats2_closure$def to i64
  %lnctX = add i64 %lnctW, 1
  store i64 %lnctX, i64* %R1_Var
  %lnctZ = load i64, i64* %lsb58
  %lnctY = load i64*, i64** %Sp_Var
  %lncu0 = getelementptr inbounds i64, i64* %lnctY, i32 0
  store i64 %lnctZ, i64* %lncu0, !tbaa !1
  %lncu1 = load i64*, i64** %Sp_Var
  %lncu2 = getelementptr inbounds i64, i64* %lncu1, i32 -1
  %lncu3 = ptrtoint i64* %lncu2 to i64
  %lncu4 = inttoptr i64 %lncu3 to i64*
  store i64* %lncu4, i64** %Sp_Var
  %lncu5 = bitcast i8* @stg_newMutVarzh to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncu6 = load i64*, i64** %Sp_Var
  %lncu7 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncu5( i64* %Base_Arg, i64* %lncu6, i64* %Hp_Arg, i64 %lncu7, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccqn_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccqn_info$def to i8*)
define internal ghccc void @ccqn_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 1, i64 32}>
{
ccqn:
  %lsb5b = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncu9 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccqp_info$def to i64
  %lncu8 = load i64*, i64** %Sp_Var
  %lncua = getelementptr inbounds i64, i64* %lncu8, i32 -1
  store i64 %lncu9, i64* %lncua, !tbaa !1
  %lncub = load i64, i64* %R1_Var
  store i64 %lncub, i64* %lsb5b
  %lncuc = ptrtoint %Disk_clearzuflushzulog2_closure_struct* @Disk_clearzuflushzulog2_closure$def to i64
  %lncud = add i64 %lncuc, 1
  store i64 %lncud, i64* %R1_Var
  %lncuf = load i64, i64* %lsb5b
  %lncue = load i64*, i64** %Sp_Var
  %lncug = getelementptr inbounds i64, i64* %lncue, i32 0
  store i64 %lncuf, i64* %lncug, !tbaa !1
  %lncuh = load i64*, i64** %Sp_Var
  %lncui = getelementptr inbounds i64, i64* %lncuh, i32 -1
  %lncuj = ptrtoint i64* %lncui to i64
  %lncuk = inttoptr i64 %lncuj to i64*
  store i64* %lncuk, i64** %Sp_Var
  %lncul = bitcast i8* @stg_newMutVarzh to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncum = load i64*, i64** %Sp_Var
  %lncun = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncul( i64* %Base_Arg, i64* %lncum, i64* %Hp_Arg, i64 %lncun, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccqp_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccqp_info$def to i8*)
define internal ghccc void @ccqp_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 2, i64 32}>
{
ccqp:
  %lsb5e = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncup = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccqr_info$def to i64
  %lncuo = load i64*, i64** %Sp_Var
  %lncuq = getelementptr inbounds i64, i64* %lncuo, i32 -1
  store i64 %lncup, i64* %lncuq, !tbaa !1
  %lncur = load i64, i64* %R1_Var
  store i64 %lncur, i64* %lsb5e
  %lncus = ptrtoint %Disk_initzudisk2_closure_struct* @Disk_initzudisk2_closure$def to i64
  %lncut = add i64 %lncus, 1
  store i64 %lncut, i64* %R1_Var
  %lncuv = load i64, i64* %lsb5e
  %lncuu = load i64*, i64** %Sp_Var
  %lncuw = getelementptr inbounds i64, i64* %lncuu, i32 0
  store i64 %lncuv, i64* %lncuw, !tbaa !1
  %lncux = load i64*, i64** %Sp_Var
  %lncuy = getelementptr inbounds i64, i64* %lncux, i32 -1
  %lncuz = ptrtoint i64* %lncuy to i64
  %lncuA = inttoptr i64 %lncuz to i64*
  store i64* %lncuA, i64** %Sp_Var
  %lncuB = bitcast i8* @stg_newMutVarzh to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncuC = load i64*, i64** %Sp_Var
  %lncuD = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncuB( i64* %Base_Arg, i64* %lncuC, i64* %Hp_Arg, i64 %lncuD, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccqr_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccqr_info$def to i8*)
define internal ghccc void @ccqr_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 3, i64 32}>
{
ccqr:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncuE = load i64*, i64** %Hp_Var
  %lncuF = getelementptr inbounds i64, i64* %lncuE, i32 6
  %lncuG = ptrtoint i64* %lncuF to i64
  %lncuH = inttoptr i64 %lncuG to i64*
  store i64* %lncuH, i64** %Hp_Var
  %lncuI = load i64*, i64** %Hp_Var
  %lncuJ = ptrtoint i64* %lncuI to i64
  %lncuK = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lncuL = bitcast i64* %lncuK to i64*
  %lncuM = load i64, i64* %lncuL, !tbaa !4
  %lncuN = icmp ugt i64 %lncuJ, %lncuM
  br i1 %lncuN, label %ccqU, label %ccqT
ccqT:
  %lncuP = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb5m_info$def to i64
  %lncuO = load i64*, i64** %Hp_Var
  %lncuQ = getelementptr inbounds i64, i64* %lncuO, i32 -5
  store i64 %lncuP, i64* %lncuQ, !tbaa !2
  %lncuS = load i64*, i64** %Sp_Var
  %lncuT = getelementptr inbounds i64, i64* %lncuS, i32 3
  %lncuU = bitcast i64* %lncuT to i64*
  %lncuV = load i64, i64* %lncuU, !tbaa !1
  %lncuR = load i64*, i64** %Hp_Var
  %lncuW = getelementptr inbounds i64, i64* %lncuR, i32 -3
  store i64 %lncuV, i64* %lncuW, !tbaa !2
  %lncuY = load i64*, i64** %Sp_Var
  %lncuZ = getelementptr inbounds i64, i64* %lncuY, i32 2
  %lncv0 = bitcast i64* %lncuZ to i64*
  %lncv1 = load i64, i64* %lncv0, !tbaa !1
  %lncuX = load i64*, i64** %Hp_Var
  %lncv2 = getelementptr inbounds i64, i64* %lncuX, i32 -2
  store i64 %lncv1, i64* %lncv2, !tbaa !2
  %lncv4 = load i64*, i64** %Sp_Var
  %lncv5 = getelementptr inbounds i64, i64* %lncv4, i32 1
  %lncv6 = bitcast i64* %lncv5 to i64*
  %lncv7 = load i64, i64* %lncv6, !tbaa !1
  %lncv3 = load i64*, i64** %Hp_Var
  %lncv8 = getelementptr inbounds i64, i64* %lncv3, i32 -1
  store i64 %lncv7, i64* %lncv8, !tbaa !2
  %lncva = load i64, i64* %R1_Var
  %lncv9 = load i64*, i64** %Hp_Var
  %lncvb = getelementptr inbounds i64, i64* %lncv9, i32 0
  store i64 %lncva, i64* %lncvb, !tbaa !2
  %lncvc = load i64*, i64** %Hp_Var
  %lncvd = getelementptr inbounds i64, i64* %lncvc, i32 -5
  %lncve = ptrtoint i64* %lncvd to i64
  store i64 %lncve, i64* %R1_Var
  %lncvf = load i64*, i64** %Sp_Var
  %lncvg = getelementptr inbounds i64, i64* %lncvf, i32 4
  %lncvh = ptrtoint i64* %lncvg to i64
  %lncvi = inttoptr i64 %lncvh to i64*
  store i64* %lncvi, i64** %Sp_Var
  %lncvj = load i64*, i64** %Sp_Var
  %lncvk = getelementptr inbounds i64, i64* %lncvj, i32 0
  %lncvl = bitcast i64* %lncvk to i64*
  %lncvm = load i64, i64* %lncvl, !tbaa !1
  %lncvn = inttoptr i64 %lncvm to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncvo = load i64*, i64** %Sp_Var
  %lncvp = load i64*, i64** %Hp_Var
  %lncvq = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncvn( i64* %Base_Arg, i64* %lncvo, i64* %lncvp, i64 %lncvq, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccqU:
  %lncvr = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 48, i64* %lncvr, !tbaa !4
  %lncvs = load i64, i64* %R1_Var
  store i64 %lncvs, i64* %R1_Var
  %lncvt = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncvu = load i64*, i64** %Sp_Var
  %lncvv = load i64*, i64** %Hp_Var
  %lncvw = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncvt( i64* %Base_Arg, i64* %lncvu, i64* %lncvv, i64 %lncvw, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_initzudiskzucrashlog_closure_struct = type <{i64, i64}>
@Disk_initzudiskzucrashlog_closure$def = internal global %Disk_initzudiskzucrashlog_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_initzudiskzucrashlog_info$def to i64), i64 0}>
@Disk_initzudiskzucrashlog_closure = alias i8* bitcast (%Disk_initzudiskzucrashlog_closure_struct* @Disk_initzudiskzucrashlog_closure$def to i8*)
@Disk_initzudiskzucrashlog_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_initzudiskzucrashlog_info$def to i8*)
define ghccc void @Disk_initzudiskzucrashlog_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_initzudiskzucrashlog_info$def to i64)),i64 216), i64 8589934597, i64 0, i64 4294967311}>
{
ccvC:
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncvF = load i64, i64* %R2_Var
  store i64 %lncvF, i64* %R2_Var
  %lncvG = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_initzudiskzucrashlog1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncvH = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncvG( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lncvH, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_initzudisk1_closure_struct = type <{i64, i64}>
@Disk_initzudisk1_closure$def = internal global %Disk_initzudisk1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_initzudisk1_info$def to i64), i64 0}>
@Disk_initzudisk1_closure = alias i8* bitcast (%Disk_initzudisk1_closure_struct* @Disk_initzudisk1_closure$def to i8*)
@sb5A_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb5A_info$def to i8*)
define internal ghccc void @sb5A_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 3, i64 16}>
{
ccw7:
  %lsb5u = alloca i64, i32 1
  %lsb5x = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncwp = load i64*, i64** %Sp_Var
  %lncwq = getelementptr inbounds i64, i64* %lncwp, i32 -5
  %lncwr = ptrtoint i64* %lncwq to i64
  %lncws = icmp ult i64 %lncwr, %SpLim_Arg
  br i1 %lncws, label %ccwb, label %ccwc
ccwc:
  %lncwu = ptrtoint i8* @stg_upd_frame_info to i64
  %lncwt = load i64*, i64** %Sp_Var
  %lncwv = getelementptr inbounds i64, i64* %lncwt, i32 -2
  store i64 %lncwu, i64* %lncwv, !tbaa !1
  %lncwx = load i64, i64* %R1_Var
  %lncww = load i64*, i64** %Sp_Var
  %lncwy = getelementptr inbounds i64, i64* %lncww, i32 -1
  store i64 %lncwx, i64* %lncwy, !tbaa !1
  %lncwA = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccw4_info$def to i64
  %lncwz = load i64*, i64** %Sp_Var
  %lncwB = getelementptr inbounds i64, i64* %lncwz, i32 -5
  store i64 %lncwA, i64* %lncwB, !tbaa !1
  %lncwE = load i64, i64* %R1_Var
  %lncwF = add i64 %lncwE, 24
  %lncwG = inttoptr i64 %lncwF to i64*
  %lncwH = load i64, i64* %lncwG, !tbaa !3
  store i64 %lncwH, i64* %lsb5u
  %lncwK = load i64, i64* %R1_Var
  %lncwL = add i64 %lncwK, 32
  %lncwM = inttoptr i64 %lncwL to i64*
  %lncwN = load i64, i64* %lncwM, !tbaa !3
  store i64 %lncwN, i64* %lsb5x
  %lncwQ = load i64, i64* %R1_Var
  %lncwR = add i64 %lncwQ, 16
  %lncwS = inttoptr i64 %lncwR to i64*
  %lncwT = load i64, i64* %lncwS, !tbaa !3
  store i64 %lncwT, i64* %R1_Var
  %lncwV = load i64, i64* %lsb5u
  %lncwU = load i64*, i64** %Sp_Var
  %lncwW = getelementptr inbounds i64, i64* %lncwU, i32 -4
  store i64 %lncwV, i64* %lncwW, !tbaa !1
  %lncwY = load i64, i64* %lsb5x
  %lncwX = load i64*, i64** %Sp_Var
  %lncwZ = getelementptr inbounds i64, i64* %lncwX, i32 -3
  store i64 %lncwY, i64* %lncwZ, !tbaa !1
  %lncx0 = load i64*, i64** %Sp_Var
  %lncx1 = getelementptr inbounds i64, i64* %lncx0, i32 -5
  %lncx2 = ptrtoint i64* %lncx1 to i64
  %lncx3 = inttoptr i64 %lncx2 to i64*
  store i64* %lncx3, i64** %Sp_Var
  %lncx4 = load i64, i64* %R1_Var
  %lncx5 = and i64 %lncx4, 7
  %lncx6 = icmp ne i64 %lncx5, 0
  br i1 %lncx6, label %ucwg, label %ccw5
ccw5:
  %lncx8 = load i64, i64* %R1_Var
  %lncx9 = inttoptr i64 %lncx8 to i64*
  %lncxa = load i64, i64* %lncx9, !tbaa !3
  %lncxb = inttoptr i64 %lncxa to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncxc = load i64*, i64** %Sp_Var
  %lncxd = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncxb( i64* %Base_Arg, i64* %lncxc, i64* %Hp_Arg, i64 %lncxd, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucwg:
  %lncxe = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccw4_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncxf = load i64*, i64** %Sp_Var
  %lncxg = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncxe( i64* %Base_Arg, i64* %lncxf, i64* %Hp_Arg, i64 %lncxg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccwb:
  %lncxh = load i64, i64* %R1_Var
  store i64 %lncxh, i64* %R1_Var
  %lncxi = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lncxj = bitcast i64* %lncxi to i64*
  %lncxk = load i64, i64* %lncxj, !tbaa !4
  %lncxl = inttoptr i64 %lncxk to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncxm = load i64*, i64** %Sp_Var
  %lncxn = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncxl( i64* %Base_Arg, i64* %lncxm, i64* %Hp_Arg, i64 %lncxn, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccw4_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccw4_info$def to i8*)
define internal ghccc void @ccw4_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 2, i64 32}>
{
ccw4:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb5z = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncxo = load i64*, i64** %Hp_Var
  %lncxp = getelementptr inbounds i64, i64* %lncxo, i32 5
  %lncxq = ptrtoint i64* %lncxp to i64
  %lncxr = inttoptr i64 %lncxq to i64*
  store i64* %lncxr, i64** %Hp_Var
  %lncxs = load i64*, i64** %Hp_Var
  %lncxt = ptrtoint i64* %lncxs to i64
  %lncxu = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lncxv = bitcast i64* %lncxu to i64*
  %lncxw = load i64, i64* %lncxv, !tbaa !4
  %lncxx = icmp ugt i64 %lncxt, %lncxw
  br i1 %lncxx, label %ccwf, label %ccwe
ccwe:
  %lncxA = load i64, i64* %R1_Var
  %lncxB = add i64 %lncxA, 7
  %lncxC = inttoptr i64 %lncxB to i64*
  %lncxD = load i64, i64* %lncxC, !tbaa !3
  store i64 %lncxD, i64* %lsb5z
  %lncxF = ptrtoint i8* @Disk_S_con_info to i64
  %lncxE = load i64*, i64** %Hp_Var
  %lncxG = getelementptr inbounds i64, i64* %lncxE, i32 -4
  store i64 %lncxF, i64* %lncxG, !tbaa !2
  %lncxI = load i64*, i64** %Sp_Var
  %lncxJ = getelementptr inbounds i64, i64* %lncxI, i32 1
  %lncxK = bitcast i64* %lncxJ to i64*
  %lncxL = load i64, i64* %lncxK, !tbaa !1
  %lncxH = load i64*, i64** %Hp_Var
  %lncxM = getelementptr inbounds i64, i64* %lncxH, i32 -3
  store i64 %lncxL, i64* %lncxM, !tbaa !2
  %lncxO = ptrtoint i8* @base_GHCziBase_Nothing_closure to i64
  %lncxP = add i64 %lncxO, 1
  %lncxN = load i64*, i64** %Hp_Var
  %lncxQ = getelementptr inbounds i64, i64* %lncxN, i32 -2
  store i64 %lncxP, i64* %lncxQ, !tbaa !2
  %lncxS = load i64*, i64** %Sp_Var
  %lncxT = getelementptr inbounds i64, i64* %lncxS, i32 2
  %lncxU = bitcast i64* %lncxT to i64*
  %lncxV = load i64, i64* %lncxU, !tbaa !1
  %lncxR = load i64*, i64** %Hp_Var
  %lncxW = getelementptr inbounds i64, i64* %lncxR, i32 -1
  store i64 %lncxV, i64* %lncxW, !tbaa !2
  %lncxY = load i64, i64* %lsb5z
  %lncxX = load i64*, i64** %Hp_Var
  %lncxZ = getelementptr inbounds i64, i64* %lncxX, i32 0
  store i64 %lncxY, i64* %lncxZ, !tbaa !2
  %lncy1 = load i64*, i64** %Hp_Var
  %lncy2 = ptrtoint i64* %lncy1 to i64
  %lncy3 = add i64 %lncy2, -31
  store i64 %lncy3, i64* %R1_Var
  %lncy4 = load i64*, i64** %Sp_Var
  %lncy5 = getelementptr inbounds i64, i64* %lncy4, i32 3
  %lncy6 = ptrtoint i64* %lncy5 to i64
  %lncy7 = inttoptr i64 %lncy6 to i64*
  store i64* %lncy7, i64** %Sp_Var
  %lncy8 = load i64*, i64** %Sp_Var
  %lncy9 = getelementptr inbounds i64, i64* %lncy8, i32 0
  %lncya = bitcast i64* %lncy9 to i64*
  %lncyb = load i64, i64* %lncya, !tbaa !1
  %lncyc = inttoptr i64 %lncyb to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncyd = load i64*, i64** %Sp_Var
  %lncye = load i64*, i64** %Hp_Var
  %lncyf = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncyc( i64* %Base_Arg, i64* %lncyd, i64* %lncye, i64 %lncyf, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccwf:
  %lncyg = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 40, i64* %lncyg, !tbaa !4
  %lncyh = load i64, i64* %R1_Var
  store i64 %lncyh, i64* %R1_Var
  %lncyi = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncyj = load i64*, i64** %Sp_Var
  %lncyk = load i64*, i64** %Hp_Var
  %lncyl = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncyi( i64* %Base_Arg, i64* %lncyj, i64* %lncyk, i64 %lncyl, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@Disk_initzudisk1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_initzudisk1_info$def to i8*)
define ghccc void @Disk_initzudisk1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_initzudisk1_info$def to i64)),i64 224), i64 8589934597, i64 0, i64 12884901903}>
{
ccwh:
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lncym = load i64*, i64** %Sp_Var
  %lncyn = getelementptr inbounds i64, i64* %lncym, i32 -3
  %lncyo = ptrtoint i64* %lncyn to i64
  %lncyp = icmp ult i64 %lncyo, %SpLim_Arg
  br i1 %lncyp, label %ccwi, label %ccwj
ccwj:
  %lncyr = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccvU_info$def to i64
  %lncyq = load i64*, i64** %Sp_Var
  %lncys = getelementptr inbounds i64, i64* %lncyq, i32 -1
  store i64 %lncyr, i64* %lncys, !tbaa !1
  %lncyt = ptrtoint i8* @unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_defaultFileFlags_closure to i64
  store i64 %lncyt, i64* %R5_Var
  %lncyu = ptrtoint %Disk_initzudisk4_closure_struct* @Disk_initzudisk4_closure$def to i64
  %lncyv = add i64 %lncyu, 2
  store i64 %lncyv, i64* %R4_Var
  %lncyw = ptrtoint i8* @unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_ReadWrite_closure to i64
  %lncyx = add i64 %lncyw, 3
  store i64 %lncyx, i64* %R3_Var
  %lncyy = load i64, i64* %R2_Var
  store i64 %lncyy, i64* %R2_Var
  %lncyz = load i64*, i64** %Sp_Var
  %lncyA = getelementptr inbounds i64, i64* %lncyz, i32 -1
  %lncyB = ptrtoint i64* %lncyA to i64
  %lncyC = inttoptr i64 %lncyB to i64*
  store i64* %lncyC, i64** %Sp_Var
  %lncyD = bitcast i8* @unixzm2zi7zi2zi0_SystemziPosixziIO_createFile3_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncyE = load i64*, i64** %Sp_Var
  %lncyF = load i64, i64* %R1_Var
  %lncyG = load i64, i64* %R2_Var
  %lncyH = load i64, i64* %R3_Var
  %lncyI = load i64, i64* %R4_Var
  %lncyJ = load i64, i64* %R5_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncyD( i64* %Base_Arg, i64* %lncyE, i64* %Hp_Arg, i64 %lncyF, i64 %lncyG, i64 %lncyH, i64 %lncyI, i64 %lncyJ, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccwi:
  %lncyK = load i64, i64* %R2_Var
  store i64 %lncyK, i64* %R2_Var
  %lncyL = ptrtoint %Disk_initzudisk1_closure_struct* @Disk_initzudisk1_closure$def to i64
  store i64 %lncyL, i64* %R1_Var
  %lncyM = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lncyN = bitcast i64* %lncyM to i64*
  %lncyO = load i64, i64* %lncyN, !tbaa !4
  %lncyP = inttoptr i64 %lncyO to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncyQ = load i64*, i64** %Sp_Var
  %lncyR = load i64, i64* %R1_Var
  %lncyS = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncyP( i64* %Base_Arg, i64* %lncyQ, i64* %Hp_Arg, i64 %lncyR, i64 %lncyS, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccvU_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccvU_info$def to i8*)
define internal ghccc void @ccvU_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
ccvU:
  %lsb5r = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncyU = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccvW_info$def to i64
  %lncyT = load i64*, i64** %Sp_Var
  %lncyV = getelementptr inbounds i64, i64* %lncyT, i32 -1
  store i64 %lncyU, i64* %lncyV, !tbaa !1
  %lncyW = load i64, i64* %R1_Var
  store i64 %lncyW, i64* %lsb5r
  %lncyX = ptrtoint %Disk_clearzustats2_closure_struct* @Disk_clearzustats2_closure$def to i64
  %lncyY = add i64 %lncyX, 1
  store i64 %lncyY, i64* %R1_Var
  %lncz0 = load i64, i64* %lsb5r
  %lncyZ = load i64*, i64** %Sp_Var
  %lncz1 = getelementptr inbounds i64, i64* %lncyZ, i32 0
  store i64 %lncz0, i64* %lncz1, !tbaa !1
  %lncz2 = load i64*, i64** %Sp_Var
  %lncz3 = getelementptr inbounds i64, i64* %lncz2, i32 -1
  %lncz4 = ptrtoint i64* %lncz3 to i64
  %lncz5 = inttoptr i64 %lncz4 to i64*
  store i64* %lncz5, i64** %Sp_Var
  %lncz6 = bitcast i8* @stg_newMutVarzh to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncz7 = load i64*, i64** %Sp_Var
  %lncz8 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncz6( i64* %Base_Arg, i64* %lncz7, i64* %Hp_Arg, i64 %lncz8, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccvW_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccvW_info$def to i8*)
define internal ghccc void @ccvW_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 1, i64 32}>
{
ccvW:
  %lsb5u = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncza = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccvY_info$def to i64
  %lncz9 = load i64*, i64** %Sp_Var
  %lnczb = getelementptr inbounds i64, i64* %lncz9, i32 -1
  store i64 %lncza, i64* %lnczb, !tbaa !1
  %lnczc = load i64, i64* %R1_Var
  store i64 %lnczc, i64* %lsb5u
  %lnczd = ptrtoint %Disk_initzudisk2_closure_struct* @Disk_initzudisk2_closure$def to i64
  %lncze = add i64 %lnczd, 1
  store i64 %lncze, i64* %R1_Var
  %lnczg = load i64, i64* %lsb5u
  %lnczf = load i64*, i64** %Sp_Var
  %lnczh = getelementptr inbounds i64, i64* %lnczf, i32 0
  store i64 %lnczg, i64* %lnczh, !tbaa !1
  %lnczi = load i64*, i64** %Sp_Var
  %lnczj = getelementptr inbounds i64, i64* %lnczi, i32 -1
  %lnczk = ptrtoint i64* %lnczj to i64
  %lnczl = inttoptr i64 %lnczk to i64*
  store i64* %lnczl, i64** %Sp_Var
  %lnczm = bitcast i8* @stg_newMutVarzh to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnczn = load i64*, i64** %Sp_Var
  %lnczo = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnczm( i64* %Base_Arg, i64* %lnczn, i64* %Hp_Arg, i64 %lnczo, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccvY_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccvY_info$def to i8*)
define internal ghccc void @ccvY_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 2, i64 32}>
{
ccvY:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnczp = load i64*, i64** %Hp_Var
  %lnczq = getelementptr inbounds i64, i64* %lnczp, i32 5
  %lnczr = ptrtoint i64* %lnczq to i64
  %lnczs = inttoptr i64 %lnczr to i64*
  store i64* %lnczs, i64** %Hp_Var
  %lnczt = load i64*, i64** %Hp_Var
  %lnczu = ptrtoint i64* %lnczt to i64
  %lnczv = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnczw = bitcast i64* %lnczv to i64*
  %lnczx = load i64, i64* %lnczw, !tbaa !4
  %lnczy = icmp ugt i64 %lnczu, %lnczx
  br i1 %lnczy, label %ccwo, label %ccwn
ccwn:
  %lnczA = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb5A_info$def to i64
  %lnczz = load i64*, i64** %Hp_Var
  %lnczB = getelementptr inbounds i64, i64* %lnczz, i32 -4
  store i64 %lnczA, i64* %lnczB, !tbaa !2
  %lnczD = load i64*, i64** %Sp_Var
  %lnczE = getelementptr inbounds i64, i64* %lnczD, i32 2
  %lnczF = bitcast i64* %lnczE to i64*
  %lnczG = load i64, i64* %lnczF, !tbaa !1
  %lnczC = load i64*, i64** %Hp_Var
  %lnczH = getelementptr inbounds i64, i64* %lnczC, i32 -2
  store i64 %lnczG, i64* %lnczH, !tbaa !2
  %lnczJ = load i64*, i64** %Sp_Var
  %lnczK = getelementptr inbounds i64, i64* %lnczJ, i32 1
  %lnczL = bitcast i64* %lnczK to i64*
  %lnczM = load i64, i64* %lnczL, !tbaa !1
  %lnczI = load i64*, i64** %Hp_Var
  %lnczN = getelementptr inbounds i64, i64* %lnczI, i32 -1
  store i64 %lnczM, i64* %lnczN, !tbaa !2
  %lnczP = load i64, i64* %R1_Var
  %lnczO = load i64*, i64** %Hp_Var
  %lnczQ = getelementptr inbounds i64, i64* %lnczO, i32 0
  store i64 %lnczP, i64* %lnczQ, !tbaa !2
  %lnczR = load i64*, i64** %Hp_Var
  %lnczS = getelementptr inbounds i64, i64* %lnczR, i32 -4
  %lnczT = ptrtoint i64* %lnczS to i64
  store i64 %lnczT, i64* %R1_Var
  %lnczU = load i64*, i64** %Sp_Var
  %lnczV = getelementptr inbounds i64, i64* %lnczU, i32 3
  %lnczW = ptrtoint i64* %lnczV to i64
  %lnczX = inttoptr i64 %lnczW to i64*
  store i64* %lnczX, i64** %Sp_Var
  %lnczY = load i64*, i64** %Sp_Var
  %lnczZ = getelementptr inbounds i64, i64* %lnczY, i32 0
  %lncA0 = bitcast i64* %lnczZ to i64*
  %lncA1 = load i64, i64* %lncA0, !tbaa !1
  %lncA2 = inttoptr i64 %lncA1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncA3 = load i64*, i64** %Sp_Var
  %lncA4 = load i64*, i64** %Hp_Var
  %lncA5 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncA2( i64* %Base_Arg, i64* %lncA3, i64* %lncA4, i64 %lncA5, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccwo:
  %lncA6 = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 40, i64* %lncA6, !tbaa !4
  %lncA7 = load i64, i64* %R1_Var
  store i64 %lncA7, i64* %R1_Var
  %lncA8 = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncA9 = load i64*, i64** %Sp_Var
  %lncAa = load i64*, i64** %Hp_Var
  %lncAb = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncA8( i64* %Base_Arg, i64* %lncA9, i64* %lncAa, i64 %lncAb, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_initzudisk_closure_struct = type <{i64, i64}>
@Disk_initzudisk_closure$def = internal global %Disk_initzudisk_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_initzudisk_info$def to i64), i64 0}>
@Disk_initzudisk_closure = alias i8* bitcast (%Disk_initzudisk_closure_struct* @Disk_initzudisk_closure$def to i8*)
@Disk_initzudisk_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_initzudisk_info$def to i8*)
define ghccc void @Disk_initzudisk_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_initzudisk_info$def to i64)),i64 232), i64 8589934597, i64 0, i64 4294967311}>
{
ccAh:
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncAk = load i64, i64* %R2_Var
  store i64 %lncAk, i64* %R2_Var
  %lncAl = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_initzudisk1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncAm = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncAl( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lncAm, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_getzustats1_closure_struct = type <{i64}>
@Disk_getzustats1_closure$def = internal global %Disk_getzustats1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_getzustats1_info$def to i64)}>
@Disk_getzustats1_closure = alias i8* bitcast (%Disk_getzustats1_closure_struct* @Disk_getzustats1_closure$def to i8*)
@Disk_getzustats1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_getzustats1_info$def to i8*)
define ghccc void @Disk_getzustats1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934597, i64 0, i64 15}>
{
ccAy:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncAG = load i64*, i64** %Sp_Var
  %lncAH = getelementptr inbounds i64, i64* %lncAG, i32 -1
  %lncAI = ptrtoint i64* %lncAH to i64
  %lncAJ = icmp ult i64 %lncAI, %SpLim_Arg
  br i1 %lncAJ, label %ccAC, label %ccAD
ccAD:
  %lncAL = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccAv_info$def to i64
  %lncAK = load i64*, i64** %Sp_Var
  %lncAM = getelementptr inbounds i64, i64* %lncAK, i32 -1
  store i64 %lncAL, i64* %lncAM, !tbaa !1
  %lncAN = load i64, i64* %R2_Var
  store i64 %lncAN, i64* %R1_Var
  %lncAO = load i64*, i64** %Sp_Var
  %lncAP = getelementptr inbounds i64, i64* %lncAO, i32 -1
  %lncAQ = ptrtoint i64* %lncAP to i64
  %lncAR = inttoptr i64 %lncAQ to i64*
  store i64* %lncAR, i64** %Sp_Var
  %lncAS = load i64, i64* %R1_Var
  %lncAT = and i64 %lncAS, 7
  %lncAU = icmp ne i64 %lncAT, 0
  br i1 %lncAU, label %ucAF, label %ccAw
ccAw:
  %lncAW = load i64, i64* %R1_Var
  %lncAX = inttoptr i64 %lncAW to i64*
  %lncAY = load i64, i64* %lncAX, !tbaa !3
  %lncAZ = inttoptr i64 %lncAY to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncB0 = load i64*, i64** %Sp_Var
  %lncB1 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncAZ( i64* %Base_Arg, i64* %lncB0, i64* %Hp_Arg, i64 %lncB1, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucAF:
  %lncB2 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccAv_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncB3 = load i64*, i64** %Sp_Var
  %lncB4 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncB2( i64* %Base_Arg, i64* %lncB3, i64* %Hp_Arg, i64 %lncB4, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccAC:
  %lncB5 = load i64, i64* %R2_Var
  store i64 %lncB5, i64* %R2_Var
  %lncB6 = ptrtoint %Disk_getzustats1_closure_struct* @Disk_getzustats1_closure$def to i64
  store i64 %lncB6, i64* %R1_Var
  %lncB7 = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lncB8 = bitcast i64* %lncB7 to i64*
  %lncB9 = load i64, i64* %lncB8, !tbaa !4
  %lncBa = inttoptr i64 %lncB9 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncBb = load i64*, i64** %Sp_Var
  %lncBc = load i64, i64* %R1_Var
  %lncBd = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncBa( i64* %Base_Arg, i64* %lncBb, i64* %Hp_Arg, i64 %lncBc, i64 %lncBd, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccAv_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccAv_info$def to i8*)
define internal ghccc void @ccAv_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
ccAv:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncBg = load i64, i64* %R1_Var
  %lncBh = add i64 %lncBg, 7
  %lncBi = inttoptr i64 %lncBh to i64*
  %lncBj = load i64, i64* %lncBi, !tbaa !3
  %lncBk = add i64 %lncBj, 8
  %lncBl = inttoptr i64 %lncBk to i64*
  %lncBm = load i64, i64* %lncBl, !tbaa !5
  store i64 %lncBm, i64* %R1_Var
  %lncBn = load i64*, i64** %Sp_Var
  %lncBo = getelementptr inbounds i64, i64* %lncBn, i32 1
  %lncBp = ptrtoint i64* %lncBo to i64
  %lncBq = inttoptr i64 %lncBp to i64*
  store i64* %lncBq, i64** %Sp_Var
  %lncBr = load i64*, i64** %Sp_Var
  %lncBs = getelementptr inbounds i64, i64* %lncBr, i32 0
  %lncBt = bitcast i64* %lncBs to i64*
  %lncBu = load i64, i64* %lncBt, !tbaa !1
  %lncBv = inttoptr i64 %lncBu to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncBw = load i64*, i64** %Sp_Var
  %lncBx = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncBv( i64* %Base_Arg, i64* %lncBw, i64* %Hp_Arg, i64 %lncBx, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_getzustats_closure_struct = type <{i64}>
@Disk_getzustats_closure$def = internal global %Disk_getzustats_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_getzustats_info$def to i64)}>
@Disk_getzustats_closure = alias i8* bitcast (%Disk_getzustats_closure_struct* @Disk_getzustats_closure$def to i8*)
@Disk_getzustats_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_getzustats_info$def to i8*)
define ghccc void @Disk_getzustats_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934597, i64 0, i64 15}>
{
ccBD:
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncBG = load i64, i64* %R2_Var
  store i64 %lncBG, i64* %R2_Var
  %lncBH = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_getzustats1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncBI = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncBH( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lncBI, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_logFlush1_closure_struct = type <{i64}>
@Disk_logFlush1_closure$def = internal global %Disk_logFlush1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_logFlush1_info$def to i64)}>
@Disk_logFlush1_closure = alias i8* bitcast (%Disk_logFlush1_closure_struct* @Disk_logFlush1_closure$def to i8*)
@Disk_logFlush1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_logFlush1_info$def to i8*)
define ghccc void @Disk_logFlush1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934597, i64 0, i64 15}>
{
ccCb:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncCG = load i64*, i64** %Sp_Var
  %lncCH = getelementptr inbounds i64, i64* %lncCG, i32 -2
  %lncCI = ptrtoint i64* %lncCH to i64
  %lncCJ = icmp ult i64 %lncCI, %SpLim_Arg
  br i1 %lncCJ, label %ccCc, label %ccCd
ccCd:
  %lncCL = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccC4_info$def to i64
  %lncCK = load i64*, i64** %Sp_Var
  %lncCM = getelementptr inbounds i64, i64* %lncCK, i32 -1
  store i64 %lncCL, i64* %lncCM, !tbaa !1
  %lncCN = load i64, i64* %R2_Var
  store i64 %lncCN, i64* %R1_Var
  %lncCO = load i64*, i64** %Sp_Var
  %lncCP = getelementptr inbounds i64, i64* %lncCO, i32 -1
  %lncCQ = ptrtoint i64* %lncCP to i64
  %lncCR = inttoptr i64 %lncCQ to i64*
  store i64* %lncCR, i64** %Sp_Var
  %lncCS = load i64, i64* %R1_Var
  %lncCT = and i64 %lncCS, 7
  %lncCU = icmp ne i64 %lncCT, 0
  br i1 %lncCU, label %ucCE, label %ccC5
ccC5:
  %lncCW = load i64, i64* %R1_Var
  %lncCX = inttoptr i64 %lncCW to i64*
  %lncCY = load i64, i64* %lncCX, !tbaa !3
  %lncCZ = inttoptr i64 %lncCY to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncD0 = load i64*, i64** %Sp_Var
  %lncD1 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncCZ( i64* %Base_Arg, i64* %lncD0, i64* %Hp_Arg, i64 %lncD1, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucCE:
  %lncD2 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccC4_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncD3 = load i64*, i64** %Sp_Var
  %lncD4 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncD2( i64* %Base_Arg, i64* %lncD3, i64* %Hp_Arg, i64 %lncD4, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccCc:
  %lncD5 = load i64, i64* %R2_Var
  store i64 %lncD5, i64* %R2_Var
  %lncD6 = ptrtoint %Disk_logFlush1_closure_struct* @Disk_logFlush1_closure$def to i64
  store i64 %lncD6, i64* %R1_Var
  %lncD7 = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lncD8 = bitcast i64* %lncD7 to i64*
  %lncD9 = load i64, i64* %lncD8, !tbaa !4
  %lncDa = inttoptr i64 %lncD9 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncDb = load i64*, i64** %Sp_Var
  %lncDc = load i64, i64* %R1_Var
  %lncDd = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncDa( i64* %Base_Arg, i64* %lncDb, i64* %Hp_Arg, i64 %lncDc, i64 %lncDd, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccC4_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccC4_info$def to i8*)
define internal ghccc void @ccC4_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
ccC4:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncDe = load i64, i64* %R1_Var
  %lncDf = and i64 %lncDe, 7
  switch i64 %lncDf, label %ccC8 [i64 1, label %ccC8
i64 2, label %ccC9]
ccC8:
  %lncDg = ptrtoint i8* @ghczmprim_GHCziTuple_Z0T_closure to i64
  %lncDh = add i64 %lncDg, 1
  store i64 %lncDh, i64* %R1_Var
  %lncDi = load i64*, i64** %Sp_Var
  %lncDj = getelementptr inbounds i64, i64* %lncDi, i32 1
  %lncDk = ptrtoint i64* %lncDj to i64
  %lncDl = inttoptr i64 %lncDk to i64*
  store i64* %lncDl, i64** %Sp_Var
  %lncDm = load i64*, i64** %Sp_Var
  %lncDn = getelementptr inbounds i64, i64* %lncDm, i32 0
  %lncDo = bitcast i64* %lncDn to i64*
  %lncDp = load i64, i64* %lncDo, !tbaa !1
  %lncDq = inttoptr i64 %lncDp to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncDr = load i64*, i64** %Sp_Var
  %lncDs = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncDq( i64* %Base_Arg, i64* %lncDr, i64* %Hp_Arg, i64 %lncDs, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccC9:
  %lncDu = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccCj_info$def to i64
  %lncDt = load i64*, i64** %Sp_Var
  %lncDv = getelementptr inbounds i64, i64* %lncDt, i32 0
  store i64 %lncDu, i64* %lncDv, !tbaa !1
  %lncDy = load i64, i64* %R1_Var
  %lncDz = add i64 %lncDy, 6
  %lncDA = inttoptr i64 %lncDz to i64*
  %lncDB = load i64, i64* %lncDA, !tbaa !3
  store i64 %lncDB, i64* %R1_Var
  %lncDC = load i64, i64* %R1_Var
  %lncDD = and i64 %lncDC, 7
  %lncDE = icmp ne i64 %lncDD, 0
  br i1 %lncDE, label %ucCD, label %ccCk
ccCk:
  %lncDG = load i64, i64* %R1_Var
  %lncDH = inttoptr i64 %lncDG to i64*
  %lncDI = load i64, i64* %lncDH, !tbaa !3
  %lncDJ = inttoptr i64 %lncDI to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncDK = load i64*, i64** %Sp_Var
  %lncDL = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncDJ( i64* %Base_Arg, i64* %lncDK, i64* %Hp_Arg, i64 %lncDL, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucCD:
  %lncDM = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccCj_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncDN = load i64*, i64** %Sp_Var
  %lncDO = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncDM( i64* %Base_Arg, i64* %lncDN, i64* %Hp_Arg, i64 %lncDO, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccCj_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccCj_info$def to i8*)
define internal ghccc void @ccCj_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
ccCj:
  %lsb5N = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncDQ = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccCo_info$def to i64
  %lncDP = load i64*, i64** %Sp_Var
  %lncDR = getelementptr inbounds i64, i64* %lncDP, i32 -1
  store i64 %lncDQ, i64* %lncDR, !tbaa !1
  %lncDU = load i64, i64* %R1_Var
  %lncDV = add i64 %lncDU, 7
  %lncDW = inttoptr i64 %lncDV to i64*
  %lncDX = load i64, i64* %lncDW, !tbaa !3
  store i64 %lncDX, i64* %lsb5N
  %lncDY = load i64, i64* %lsb5N
  %lncDZ = add i64 %lncDY, 8
  %lncE0 = inttoptr i64 %lncDZ to i64*
  %lncE1 = load i64, i64* %lncE0, !tbaa !5
  store i64 %lncE1, i64* %R1_Var
  %lncE3 = load i64, i64* %lsb5N
  %lncE2 = load i64*, i64** %Sp_Var
  %lncE4 = getelementptr inbounds i64, i64* %lncE2, i32 0
  store i64 %lncE3, i64* %lncE4, !tbaa !1
  %lncE5 = load i64*, i64** %Sp_Var
  %lncE6 = getelementptr inbounds i64, i64* %lncE5, i32 -1
  %lncE7 = ptrtoint i64* %lncE6 to i64
  %lncE8 = inttoptr i64 %lncE7 to i64*
  store i64* %lncE8, i64** %Sp_Var
  %lncE9 = load i64, i64* %R1_Var
  %lncEa = and i64 %lncE9, 7
  %lncEb = icmp ne i64 %lncEa, 0
  br i1 %lncEb, label %ucCF, label %ccCr
ccCr:
  %lncEd = load i64, i64* %R1_Var
  %lncEe = inttoptr i64 %lncEd to i64*
  %lncEf = load i64, i64* %lncEe, !tbaa !3
  %lncEg = inttoptr i64 %lncEf to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncEh = load i64*, i64** %Sp_Var
  %lncEi = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncEg( i64* %Base_Arg, i64* %lncEh, i64* %Hp_Arg, i64 %lncEi, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucCF:
  %lncEj = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccCo_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncEk = load i64*, i64** %Sp_Var
  %lncEl = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncEj( i64* %Base_Arg, i64* %lncEk, i64* %Hp_Arg, i64 %lncEl, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccCo_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccCo_info$def to i8*)
define internal ghccc void @ccCo_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 1, i64 32}>
{
ccCo:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb5S = alloca i64, i32 1
  %lsb5T = alloca i64, i32 1
  %lsb5N = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncEm = load i64*, i64** %Hp_Var
  %lncEn = getelementptr inbounds i64, i64* %lncEm, i32 6
  %lncEo = ptrtoint i64* %lncEn to i64
  %lncEp = inttoptr i64 %lncEo to i64*
  store i64* %lncEp, i64** %Hp_Var
  %lncEq = load i64*, i64** %Hp_Var
  %lncEr = ptrtoint i64* %lncEq to i64
  %lncEs = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lncEt = bitcast i64* %lncEs to i64*
  %lncEu = load i64, i64* %lncEt, !tbaa !4
  %lncEv = icmp ugt i64 %lncEr, %lncEu
  br i1 %lncEv, label %ccCx, label %ccCw
ccCw:
  %lncEy = load i64, i64* %R1_Var
  %lncEz = add i64 %lncEy, 7
  %lncEA = inttoptr i64 %lncEz to i64*
  %lncEB = load i64, i64* %lncEA, !tbaa !3
  store i64 %lncEB, i64* %lsb5S
  %lncEE = load i64, i64* %R1_Var
  %lncEF = add i64 %lncEE, 15
  %lncEG = inttoptr i64 %lncEF to i64*
  %lncEH = load i64, i64* %lncEG, !tbaa !3
  store i64 %lncEH, i64* %lsb5T
  %lncEJ = ptrtoint i8* @ghczmprim_GHCziTypes_ZC_con_info to i64
  %lncEI = load i64*, i64** %Hp_Var
  %lncEK = getelementptr inbounds i64, i64* %lncEI, i32 -5
  store i64 %lncEJ, i64* %lncEK, !tbaa !2
  %lncEM = load i64, i64* %lsb5S
  %lncEL = load i64*, i64** %Hp_Var
  %lncEN = getelementptr inbounds i64, i64* %lncEL, i32 -4
  store i64 %lncEM, i64* %lncEN, !tbaa !2
  %lncEP = load i64, i64* %lsb5T
  %lncEO = load i64*, i64** %Hp_Var
  %lncEQ = getelementptr inbounds i64, i64* %lncEO, i32 -3
  store i64 %lncEP, i64* %lncEQ, !tbaa !2
  %lncES = ptrtoint i8* @Disk_FL_con_info to i64
  %lncER = load i64*, i64** %Hp_Var
  %lncET = getelementptr inbounds i64, i64* %lncER, i32 -2
  store i64 %lncES, i64* %lncET, !tbaa !2
  %lncEV = ptrtoint i8* @ghczmprim_GHCziTypes_ZMZN_closure to i64
  %lncEW = add i64 %lncEV, 1
  %lncEU = load i64*, i64** %Hp_Var
  %lncEX = getelementptr inbounds i64, i64* %lncEU, i32 -1
  store i64 %lncEW, i64* %lncEX, !tbaa !2
  %lncF0 = load i64*, i64** %Hp_Var
  %lncF1 = ptrtoint i64* %lncF0 to i64
  %lncF2 = add i64 %lncF1, -38
  %lncEY = load i64*, i64** %Hp_Var
  %lncF3 = getelementptr inbounds i64, i64* %lncEY, i32 0
  store i64 %lncF2, i64* %lncF3, !tbaa !2
  %lncF4 = load i64*, i64** %Sp_Var
  %lncF5 = getelementptr inbounds i64, i64* %lncF4, i32 1
  %lncF6 = bitcast i64* %lncF5 to i64*
  %lncF7 = load i64, i64* %lncF6, !tbaa !1
  store i64 %lncF7, i64* %lsb5N
  %lncF8 = load i64, i64* %lsb5N
  %lncF9 = add i64 %lncF8, 8
  %lncFb = load i64*, i64** %Hp_Var
  %lncFc = ptrtoint i64* %lncFb to i64
  %lncFd = add i64 %lncFc, -15
  %lncFe = inttoptr i64 %lncF9 to i64*
  store i64 %lncFd, i64* %lncFe, !tbaa !5
  %lncFf = ptrtoint i64* %Base_Arg to i64
  %lncFg = inttoptr i64 %lncFf to i8*
  %lncFh = load i64, i64* %lsb5N
  %lncFi = inttoptr i64 %lncFh to i8*
  %lncFj = bitcast i8* @dirty_MUT_VAR to void (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  call ccc void (i8*, i8*) %lncFj( i8* %lncFg, i8* %lncFi ) nounwind
  %lncFk = ptrtoint i8* @ghczmprim_GHCziTuple_Z0T_closure to i64
  %lncFl = add i64 %lncFk, 1
  store i64 %lncFl, i64* %R1_Var
  %lncFm = load i64*, i64** %Sp_Var
  %lncFn = getelementptr inbounds i64, i64* %lncFm, i32 2
  %lncFo = ptrtoint i64* %lncFn to i64
  %lncFp = inttoptr i64 %lncFo to i64*
  store i64* %lncFp, i64** %Sp_Var
  %lncFq = load i64*, i64** %Sp_Var
  %lncFr = getelementptr inbounds i64, i64* %lncFq, i32 0
  %lncFs = bitcast i64* %lncFr to i64*
  %lncFt = load i64, i64* %lncFs, !tbaa !1
  %lncFu = inttoptr i64 %lncFt to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncFv = load i64*, i64** %Sp_Var
  %lncFw = load i64*, i64** %Hp_Var
  %lncFx = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncFu( i64* %Base_Arg, i64* %lncFv, i64* %lncFw, i64 %lncFx, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccCx:
  %lncFy = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 48, i64* %lncFy, !tbaa !4
  %lncFz = load i64, i64* %R1_Var
  store i64 %lncFz, i64* %R1_Var
  %lncFA = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncFB = load i64*, i64** %Sp_Var
  %lncFC = load i64*, i64** %Hp_Var
  %lncFD = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncFA( i64* %Base_Arg, i64* %lncFB, i64* %lncFC, i64 %lncFD, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_logFlush_closure_struct = type <{i64}>
@Disk_logFlush_closure$def = internal global %Disk_logFlush_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_logFlush_info$def to i64)}>
@Disk_logFlush_closure = alias i8* bitcast (%Disk_logFlush_closure_struct* @Disk_logFlush_closure$def to i8*)
@Disk_logFlush_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_logFlush_info$def to i8*)
define ghccc void @Disk_logFlush_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934597, i64 0, i64 15}>
{
ccFJ:
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncFM = load i64, i64* %R2_Var
  store i64 %lncFM, i64* %R2_Var
  %lncFN = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_logFlush1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncFO = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncFN( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lncFO, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_logWrite1_closure_struct = type <{i64}>
@Disk_logWrite1_closure$def = internal global %Disk_logWrite1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_logWrite1_info$def to i64)}>
@Disk_logWrite1_closure = alias i8* bitcast (%Disk_logWrite1_closure_struct* @Disk_logWrite1_closure$def to i8*)
@Disk_logWrite1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_logWrite1_info$def to i8*)
define ghccc void @Disk_logWrite1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 17179869207, i64 0, i64 15}>
{
ccGi:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R4_Var = alloca i64, i32 1
  store i64 %R4_Arg, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncGO = load i64*, i64** %Sp_Var
  %lncGP = getelementptr inbounds i64, i64* %lncGO, i32 -4
  %lncGQ = ptrtoint i64* %lncGP to i64
  %lncGR = icmp ult i64 %lncGQ, %SpLim_Arg
  br i1 %lncGR, label %ccGj, label %ccGk
ccGk:
  %lncGT = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccGb_info$def to i64
  %lncGS = load i64*, i64** %Sp_Var
  %lncGU = getelementptr inbounds i64, i64* %lncGS, i32 -3
  store i64 %lncGT, i64* %lncGU, !tbaa !1
  %lncGV = load i64, i64* %R2_Var
  store i64 %lncGV, i64* %R1_Var
  %lncGX = load i64, i64* %R3_Var
  %lncGW = load i64*, i64** %Sp_Var
  %lncGY = getelementptr inbounds i64, i64* %lncGW, i32 -2
  store i64 %lncGX, i64* %lncGY, !tbaa !1
  %lncH0 = load i64, i64* %R4_Var
  %lncGZ = load i64*, i64** %Sp_Var
  %lncH1 = getelementptr inbounds i64, i64* %lncGZ, i32 -1
  store i64 %lncH0, i64* %lncH1, !tbaa !1
  %lncH2 = load i64*, i64** %Sp_Var
  %lncH3 = getelementptr inbounds i64, i64* %lncH2, i32 -3
  %lncH4 = ptrtoint i64* %lncH3 to i64
  %lncH5 = inttoptr i64 %lncH4 to i64*
  store i64* %lncH5, i64** %Sp_Var
  %lncH6 = load i64, i64* %R1_Var
  %lncH7 = and i64 %lncH6, 7
  %lncH8 = icmp ne i64 %lncH7, 0
  br i1 %lncH8, label %ucGM, label %ccGc
ccGc:
  %lncHa = load i64, i64* %R1_Var
  %lncHb = inttoptr i64 %lncHa to i64*
  %lncHc = load i64, i64* %lncHb, !tbaa !3
  %lncHd = inttoptr i64 %lncHc to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncHe = load i64*, i64** %Sp_Var
  %lncHf = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncHd( i64* %Base_Arg, i64* %lncHe, i64* %Hp_Arg, i64 %lncHf, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucGM:
  %lncHg = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccGb_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncHh = load i64*, i64** %Sp_Var
  %lncHi = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncHg( i64* %Base_Arg, i64* %lncHh, i64* %Hp_Arg, i64 %lncHi, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccGj:
  %lncHj = load i64, i64* %R4_Var
  store i64 %lncHj, i64* %R4_Var
  %lncHk = load i64, i64* %R3_Var
  store i64 %lncHk, i64* %R3_Var
  %lncHl = load i64, i64* %R2_Var
  store i64 %lncHl, i64* %R2_Var
  %lncHm = ptrtoint %Disk_logWrite1_closure_struct* @Disk_logWrite1_closure$def to i64
  store i64 %lncHm, i64* %R1_Var
  %lncHn = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lncHo = bitcast i64* %lncHn to i64*
  %lncHp = load i64, i64* %lncHo, !tbaa !4
  %lncHq = inttoptr i64 %lncHp to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncHr = load i64*, i64** %Sp_Var
  %lncHs = load i64, i64* %R1_Var
  %lncHt = load i64, i64* %R2_Var
  %lncHu = load i64, i64* %R3_Var
  %lncHv = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncHq( i64* %Base_Arg, i64* %lncHr, i64* %Hp_Arg, i64 %lncHs, i64 %lncHt, i64 %lncHu, i64 %lncHv, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccGb_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccGb_info$def to i8*)
define internal ghccc void @ccGb_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 2, i64 32}>
{
ccGb:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncHw = load i64, i64* %R1_Var
  %lncHx = and i64 %lncHw, 7
  switch i64 %lncHx, label %ccGf [i64 1, label %ccGf
i64 2, label %ccGg]
ccGf:
  %lncHy = ptrtoint i8* @ghczmprim_GHCziTuple_Z0T_closure to i64
  %lncHz = add i64 %lncHy, 1
  store i64 %lncHz, i64* %R1_Var
  %lncHA = load i64*, i64** %Sp_Var
  %lncHB = getelementptr inbounds i64, i64* %lncHA, i32 3
  %lncHC = ptrtoint i64* %lncHB to i64
  %lncHD = inttoptr i64 %lncHC to i64*
  store i64* %lncHD, i64** %Sp_Var
  %lncHE = load i64*, i64** %Sp_Var
  %lncHF = getelementptr inbounds i64, i64* %lncHE, i32 0
  %lncHG = bitcast i64* %lncHF to i64*
  %lncHH = load i64, i64* %lncHG, !tbaa !1
  %lncHI = inttoptr i64 %lncHH to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncHJ = load i64*, i64** %Sp_Var
  %lncHK = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncHI( i64* %Base_Arg, i64* %lncHJ, i64* %Hp_Arg, i64 %lncHK, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccGg:
  %lncHM = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccGq_info$def to i64
  %lncHL = load i64*, i64** %Sp_Var
  %lncHN = getelementptr inbounds i64, i64* %lncHL, i32 0
  store i64 %lncHM, i64* %lncHN, !tbaa !1
  %lncHQ = load i64, i64* %R1_Var
  %lncHR = add i64 %lncHQ, 6
  %lncHS = inttoptr i64 %lncHR to i64*
  %lncHT = load i64, i64* %lncHS, !tbaa !3
  store i64 %lncHT, i64* %R1_Var
  %lncHU = load i64, i64* %R1_Var
  %lncHV = and i64 %lncHU, 7
  %lncHW = icmp ne i64 %lncHV, 0
  br i1 %lncHW, label %ucGL, label %ccGr
ccGr:
  %lncHY = load i64, i64* %R1_Var
  %lncHZ = inttoptr i64 %lncHY to i64*
  %lncI0 = load i64, i64* %lncHZ, !tbaa !3
  %lncI1 = inttoptr i64 %lncI0 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncI2 = load i64*, i64** %Sp_Var
  %lncI3 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncI1( i64* %Base_Arg, i64* %lncI2, i64* %Hp_Arg, i64 %lncI3, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucGL:
  %lncI4 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccGq_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncI5 = load i64*, i64** %Sp_Var
  %lncI6 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncI4( i64* %Base_Arg, i64* %lncI5, i64* %Hp_Arg, i64 %lncI6, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccGq_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccGq_info$def to i8*)
define internal ghccc void @ccGq_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 2, i64 32}>
{
ccGq:
  %lsb64 = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncI8 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccGv_info$def to i64
  %lncI7 = load i64*, i64** %Sp_Var
  %lncI9 = getelementptr inbounds i64, i64* %lncI7, i32 -1
  store i64 %lncI8, i64* %lncI9, !tbaa !1
  %lncIc = load i64, i64* %R1_Var
  %lncId = add i64 %lncIc, 7
  %lncIe = inttoptr i64 %lncId to i64*
  %lncIf = load i64, i64* %lncIe, !tbaa !3
  store i64 %lncIf, i64* %lsb64
  %lncIg = load i64, i64* %lsb64
  %lncIh = add i64 %lncIg, 8
  %lncIi = inttoptr i64 %lncIh to i64*
  %lncIj = load i64, i64* %lncIi, !tbaa !5
  store i64 %lncIj, i64* %R1_Var
  %lncIl = load i64, i64* %lsb64
  %lncIk = load i64*, i64** %Sp_Var
  %lncIm = getelementptr inbounds i64, i64* %lncIk, i32 0
  store i64 %lncIl, i64* %lncIm, !tbaa !1
  %lncIn = load i64*, i64** %Sp_Var
  %lncIo = getelementptr inbounds i64, i64* %lncIn, i32 -1
  %lncIp = ptrtoint i64* %lncIo to i64
  %lncIq = inttoptr i64 %lncIp to i64*
  store i64* %lncIq, i64** %Sp_Var
  %lncIr = load i64, i64* %R1_Var
  %lncIs = and i64 %lncIr, 7
  %lncIt = icmp ne i64 %lncIs, 0
  br i1 %lncIt, label %ucGN, label %ccGy
ccGy:
  %lncIv = load i64, i64* %R1_Var
  %lncIw = inttoptr i64 %lncIv to i64*
  %lncIx = load i64, i64* %lncIw, !tbaa !3
  %lncIy = inttoptr i64 %lncIx to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncIz = load i64*, i64** %Sp_Var
  %lncIA = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncIy( i64* %Base_Arg, i64* %lncIz, i64* %Hp_Arg, i64 %lncIA, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucGN:
  %lncIB = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccGv_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncIC = load i64*, i64** %Sp_Var
  %lncID = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncIB( i64* %Base_Arg, i64* %lncIC, i64* %Hp_Arg, i64 %lncID, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccGv_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccGv_info$def to i8*)
define internal ghccc void @ccGv_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 3, i64 32}>
{
ccGv:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb69 = alloca i64, i32 1
  %lsb6a = alloca i64, i32 1
  %lsb64 = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncIE = load i64*, i64** %Hp_Var
  %lncIF = getelementptr inbounds i64, i64* %lncIE, i32 9
  %lncIG = ptrtoint i64* %lncIF to i64
  %lncIH = inttoptr i64 %lncIG to i64*
  store i64* %lncIH, i64** %Hp_Var
  %lncII = load i64*, i64** %Hp_Var
  %lncIJ = ptrtoint i64* %lncII to i64
  %lncIK = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lncIL = bitcast i64* %lncIK to i64*
  %lncIM = load i64, i64* %lncIL, !tbaa !4
  %lncIN = icmp ugt i64 %lncIJ, %lncIM
  br i1 %lncIN, label %ccGE, label %ccGD
ccGD:
  %lncIQ = load i64, i64* %R1_Var
  %lncIR = add i64 %lncIQ, 7
  %lncIS = inttoptr i64 %lncIR to i64*
  %lncIT = load i64, i64* %lncIS, !tbaa !3
  store i64 %lncIT, i64* %lsb69
  %lncIW = load i64, i64* %R1_Var
  %lncIX = add i64 %lncIW, 15
  %lncIY = inttoptr i64 %lncIX to i64*
  %lncIZ = load i64, i64* %lncIY, !tbaa !3
  store i64 %lncIZ, i64* %lsb6a
  %lncJ1 = ptrtoint i8* @ghczmprim_GHCziTuple_Z2T_con_info to i64
  %lncJ0 = load i64*, i64** %Hp_Var
  %lncJ2 = getelementptr inbounds i64, i64* %lncJ0, i32 -8
  store i64 %lncJ1, i64* %lncJ2, !tbaa !2
  %lncJ4 = load i64*, i64** %Sp_Var
  %lncJ5 = getelementptr inbounds i64, i64* %lncJ4, i32 2
  %lncJ6 = bitcast i64* %lncJ5 to i64*
  %lncJ7 = load i64, i64* %lncJ6, !tbaa !1
  %lncJ3 = load i64*, i64** %Hp_Var
  %lncJ8 = getelementptr inbounds i64, i64* %lncJ3, i32 -7
  store i64 %lncJ7, i64* %lncJ8, !tbaa !2
  %lncJa = load i64*, i64** %Sp_Var
  %lncJb = getelementptr inbounds i64, i64* %lncJa, i32 3
  %lncJc = bitcast i64* %lncJb to i64*
  %lncJd = load i64, i64* %lncJc, !tbaa !1
  %lncJ9 = load i64*, i64** %Hp_Var
  %lncJe = getelementptr inbounds i64, i64* %lncJ9, i32 -6
  store i64 %lncJd, i64* %lncJe, !tbaa !2
  %lncJg = ptrtoint i8* @ghczmprim_GHCziTypes_ZC_con_info to i64
  %lncJf = load i64*, i64** %Hp_Var
  %lncJh = getelementptr inbounds i64, i64* %lncJf, i32 -5
  store i64 %lncJg, i64* %lncJh, !tbaa !2
  %lncJk = load i64*, i64** %Hp_Var
  %lncJl = ptrtoint i64* %lncJk to i64
  %lncJm = add i64 %lncJl, -63
  %lncJi = load i64*, i64** %Hp_Var
  %lncJn = getelementptr inbounds i64, i64* %lncJi, i32 -4
  store i64 %lncJm, i64* %lncJn, !tbaa !2
  %lncJp = load i64, i64* %lsb69
  %lncJo = load i64*, i64** %Hp_Var
  %lncJq = getelementptr inbounds i64, i64* %lncJo, i32 -3
  store i64 %lncJp, i64* %lncJq, !tbaa !2
  %lncJs = ptrtoint i8* @Disk_FL_con_info to i64
  %lncJr = load i64*, i64** %Hp_Var
  %lncJt = getelementptr inbounds i64, i64* %lncJr, i32 -2
  store i64 %lncJs, i64* %lncJt, !tbaa !2
  %lncJw = load i64*, i64** %Hp_Var
  %lncJx = ptrtoint i64* %lncJw to i64
  %lncJy = add i64 %lncJx, -38
  %lncJu = load i64*, i64** %Hp_Var
  %lncJz = getelementptr inbounds i64, i64* %lncJu, i32 -1
  store i64 %lncJy, i64* %lncJz, !tbaa !2
  %lncJB = load i64, i64* %lsb6a
  %lncJA = load i64*, i64** %Hp_Var
  %lncJC = getelementptr inbounds i64, i64* %lncJA, i32 0
  store i64 %lncJB, i64* %lncJC, !tbaa !2
  %lncJD = load i64*, i64** %Sp_Var
  %lncJE = getelementptr inbounds i64, i64* %lncJD, i32 1
  %lncJF = bitcast i64* %lncJE to i64*
  %lncJG = load i64, i64* %lncJF, !tbaa !1
  store i64 %lncJG, i64* %lsb64
  %lncJH = load i64, i64* %lsb64
  %lncJI = add i64 %lncJH, 8
  %lncJK = load i64*, i64** %Hp_Var
  %lncJL = ptrtoint i64* %lncJK to i64
  %lncJM = add i64 %lncJL, -15
  %lncJN = inttoptr i64 %lncJI to i64*
  store i64 %lncJM, i64* %lncJN, !tbaa !5
  %lncJO = ptrtoint i64* %Base_Arg to i64
  %lncJP = inttoptr i64 %lncJO to i8*
  %lncJQ = load i64, i64* %lsb64
  %lncJR = inttoptr i64 %lncJQ to i8*
  %lncJS = bitcast i8* @dirty_MUT_VAR to void (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  call ccc void (i8*, i8*) %lncJS( i8* %lncJP, i8* %lncJR ) nounwind
  %lncJT = ptrtoint i8* @ghczmprim_GHCziTuple_Z0T_closure to i64
  %lncJU = add i64 %lncJT, 1
  store i64 %lncJU, i64* %R1_Var
  %lncJV = load i64*, i64** %Sp_Var
  %lncJW = getelementptr inbounds i64, i64* %lncJV, i32 4
  %lncJX = ptrtoint i64* %lncJW to i64
  %lncJY = inttoptr i64 %lncJX to i64*
  store i64* %lncJY, i64** %Sp_Var
  %lncJZ = load i64*, i64** %Sp_Var
  %lncK0 = getelementptr inbounds i64, i64* %lncJZ, i32 0
  %lncK1 = bitcast i64* %lncK0 to i64*
  %lncK2 = load i64, i64* %lncK1, !tbaa !1
  %lncK3 = inttoptr i64 %lncK2 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncK4 = load i64*, i64** %Sp_Var
  %lncK5 = load i64*, i64** %Hp_Var
  %lncK6 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncK3( i64* %Base_Arg, i64* %lncK4, i64* %lncK5, i64 %lncK6, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccGE:
  %lncK7 = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 72, i64* %lncK7, !tbaa !4
  %lncK8 = load i64, i64* %R1_Var
  store i64 %lncK8, i64* %R1_Var
  %lncK9 = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncKa = load i64*, i64** %Sp_Var
  %lncKb = load i64*, i64** %Hp_Var
  %lncKc = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncK9( i64* %Base_Arg, i64* %lncKa, i64* %lncKb, i64 %lncKc, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_logWrite_closure_struct = type <{i64}>
@Disk_logWrite_closure$def = internal global %Disk_logWrite_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_logWrite_info$def to i64)}>
@Disk_logWrite_closure = alias i8* bitcast (%Disk_logWrite_closure_struct* @Disk_logWrite_closure$def to i8*)
@Disk_logWrite_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_logWrite_info$def to i8*)
define ghccc void @Disk_logWrite_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 17179869207, i64 0, i64 15}>
{
ccKi:
  %R4_Var = alloca i64, i32 1
  store i64 %R4_Arg, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncKl = load i64, i64* %R4_Var
  store i64 %lncKl, i64* %R4_Var
  %lncKm = load i64, i64* %R3_Var
  store i64 %lncKm, i64* %R3_Var
  %lncKn = load i64, i64* %R2_Var
  store i64 %lncKn, i64* %R2_Var
  %lncKo = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_logWrite1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncKp = load i64, i64* %R2_Var
  %lncKq = load i64, i64* %R3_Var
  %lncKr = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncKo( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lncKp, i64 %lncKq, i64 %lncKr, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_bumpSync1_closure_struct = type <{i64}>
@Disk_bumpSync1_closure$def = internal global %Disk_bumpSync1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_bumpSync1_info$def to i64)}>
@Disk_bumpSync1_closure = alias i8* bitcast (%Disk_bumpSync1_closure_struct* @Disk_bumpSync1_closure$def to i8*)
@Disk_bumpSync1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_bumpSync1_info$def to i8*)
define ghccc void @Disk_bumpSync1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934597, i64 0, i64 15}>
{
ccKO:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncLb = load i64*, i64** %Sp_Var
  %lncLc = getelementptr inbounds i64, i64* %lncLb, i32 -2
  %lncLd = ptrtoint i64* %lncLc to i64
  %lncLe = icmp ult i64 %lncLd, %SpLim_Arg
  br i1 %lncLe, label %ccKS, label %ccKT
ccKT:
  %lncLg = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccKL_info$def to i64
  %lncLf = load i64*, i64** %Sp_Var
  %lncLh = getelementptr inbounds i64, i64* %lncLf, i32 -1
  store i64 %lncLg, i64* %lncLh, !tbaa !1
  %lncLi = load i64, i64* %R2_Var
  store i64 %lncLi, i64* %R1_Var
  %lncLj = load i64*, i64** %Sp_Var
  %lncLk = getelementptr inbounds i64, i64* %lncLj, i32 -1
  %lncLl = ptrtoint i64* %lncLk to i64
  %lncLm = inttoptr i64 %lncLl to i64*
  store i64* %lncLm, i64** %Sp_Var
  %lncLn = load i64, i64* %R1_Var
  %lncLo = and i64 %lncLn, 7
  %lncLp = icmp ne i64 %lncLo, 0
  br i1 %lncLp, label %ucLa, label %ccKM
ccKM:
  %lncLr = load i64, i64* %R1_Var
  %lncLs = inttoptr i64 %lncLr to i64*
  %lncLt = load i64, i64* %lncLs, !tbaa !3
  %lncLu = inttoptr i64 %lncLt to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncLv = load i64*, i64** %Sp_Var
  %lncLw = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncLu( i64* %Base_Arg, i64* %lncLv, i64* %Hp_Arg, i64 %lncLw, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucLa:
  %lncLx = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccKL_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncLy = load i64*, i64** %Sp_Var
  %lncLz = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncLx( i64* %Base_Arg, i64* %lncLy, i64* %Hp_Arg, i64 %lncLz, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccKS:
  %lncLA = load i64, i64* %R2_Var
  store i64 %lncLA, i64* %R2_Var
  %lncLB = ptrtoint %Disk_bumpSync1_closure_struct* @Disk_bumpSync1_closure$def to i64
  store i64 %lncLB, i64* %R1_Var
  %lncLC = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lncLD = bitcast i64* %lncLC to i64*
  %lncLE = load i64, i64* %lncLD, !tbaa !4
  %lncLF = inttoptr i64 %lncLE to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncLG = load i64*, i64** %Sp_Var
  %lncLH = load i64, i64* %R1_Var
  %lncLI = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncLF( i64* %Base_Arg, i64* %lncLG, i64* %Hp_Arg, i64 %lncLH, i64 %lncLI, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccKL_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccKL_info$def to i8*)
define internal ghccc void @ccKL_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
ccKL:
  %lsb6i = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncLK = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccKR_info$def to i64
  %lncLJ = load i64*, i64** %Sp_Var
  %lncLL = getelementptr inbounds i64, i64* %lncLJ, i32 -1
  store i64 %lncLK, i64* %lncLL, !tbaa !1
  %lncLO = load i64, i64* %R1_Var
  %lncLP = add i64 %lncLO, 7
  %lncLQ = inttoptr i64 %lncLP to i64*
  %lncLR = load i64, i64* %lncLQ, !tbaa !3
  store i64 %lncLR, i64* %lsb6i
  %lncLS = load i64, i64* %lsb6i
  %lncLT = add i64 %lncLS, 8
  %lncLU = inttoptr i64 %lncLT to i64*
  %lncLV = load i64, i64* %lncLU, !tbaa !5
  store i64 %lncLV, i64* %R1_Var
  %lncLX = load i64, i64* %lsb6i
  %lncLW = load i64*, i64** %Sp_Var
  %lncLY = getelementptr inbounds i64, i64* %lncLW, i32 0
  store i64 %lncLX, i64* %lncLY, !tbaa !1
  %lncLZ = load i64*, i64** %Sp_Var
  %lncM0 = getelementptr inbounds i64, i64* %lncLZ, i32 -1
  %lncM1 = ptrtoint i64* %lncM0 to i64
  %lncM2 = inttoptr i64 %lncM1 to i64*
  store i64* %lncM2, i64** %Sp_Var
  %lncM3 = load i64, i64* %R1_Var
  %lncM4 = and i64 %lncM3, 7
  %lncM5 = icmp ne i64 %lncM4, 0
  br i1 %lncM5, label %ucL9, label %ccKV
ccKV:
  %lncM7 = load i64, i64* %R1_Var
  %lncM8 = inttoptr i64 %lncM7 to i64*
  %lncM9 = load i64, i64* %lncM8, !tbaa !3
  %lncMa = inttoptr i64 %lncM9 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncMb = load i64*, i64** %Sp_Var
  %lncMc = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncMa( i64* %Base_Arg, i64* %lncMb, i64* %Hp_Arg, i64 %lncMc, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucL9:
  %lncMd = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccKR_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncMe = load i64*, i64** %Sp_Var
  %lncMf = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncMd( i64* %Base_Arg, i64* %lncMe, i64* %Hp_Arg, i64 %lncMf, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccKR_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccKR_info$def to i8*)
define internal ghccc void @ccKR_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 1, i64 32}>
{
ccKR:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb6n = alloca i64, i32 1
  %lsb6o = alloca i64, i32 1
  %lsb6q = alloca i64, i32 1
  %lsb6i = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncMg = load i64*, i64** %Hp_Var
  %lncMh = getelementptr inbounds i64, i64* %lncMg, i32 4
  %lncMi = ptrtoint i64* %lncMh to i64
  %lncMj = inttoptr i64 %lncMi to i64*
  store i64* %lncMj, i64** %Hp_Var
  %lncMk = load i64*, i64** %Hp_Var
  %lncMl = ptrtoint i64* %lncMk to i64
  %lncMm = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lncMn = bitcast i64* %lncMm to i64*
  %lncMo = load i64, i64* %lncMn, !tbaa !4
  %lncMp = icmp ugt i64 %lncMl, %lncMo
  br i1 %lncMp, label %ccL1, label %ccL0
ccL0:
  %lncMs = load i64, i64* %R1_Var
  %lncMt = add i64 %lncMs, 7
  %lncMu = inttoptr i64 %lncMt to i64*
  %lncMv = load i64, i64* %lncMu, !tbaa !3
  store i64 %lncMv, i64* %lsb6n
  %lncMy = load i64, i64* %R1_Var
  %lncMz = add i64 %lncMy, 15
  %lncMA = inttoptr i64 %lncMz to i64*
  %lncMB = load i64, i64* %lncMA, !tbaa !3
  store i64 %lncMB, i64* %lsb6o
  %lncME = load i64, i64* %R1_Var
  %lncMF = add i64 %lncME, 23
  %lncMG = inttoptr i64 %lncMF to i64*
  %lncMH = load i64, i64* %lncMG, !tbaa !3
  %lncMI = add i64 %lncMH, 1
  store i64 %lncMI, i64* %lsb6q
  %lncMK = ptrtoint i8* @Disk_Stats_con_info to i64
  %lncMJ = load i64*, i64** %Hp_Var
  %lncML = getelementptr inbounds i64, i64* %lncMJ, i32 -3
  store i64 %lncMK, i64* %lncML, !tbaa !2
  %lncMN = load i64, i64* %lsb6n
  %lncMM = load i64*, i64** %Hp_Var
  %lncMO = getelementptr inbounds i64, i64* %lncMM, i32 -2
  store i64 %lncMN, i64* %lncMO, !tbaa !2
  %lncMQ = load i64, i64* %lsb6o
  %lncMP = load i64*, i64** %Hp_Var
  %lncMR = getelementptr inbounds i64, i64* %lncMP, i32 -1
  store i64 %lncMQ, i64* %lncMR, !tbaa !2
  %lncMT = load i64, i64* %lsb6q
  %lncMS = load i64*, i64** %Hp_Var
  %lncMU = getelementptr inbounds i64, i64* %lncMS, i32 0
  store i64 %lncMT, i64* %lncMU, !tbaa !2
  %lncMV = load i64*, i64** %Sp_Var
  %lncMW = getelementptr inbounds i64, i64* %lncMV, i32 1
  %lncMX = bitcast i64* %lncMW to i64*
  %lncMY = load i64, i64* %lncMX, !tbaa !1
  store i64 %lncMY, i64* %lsb6i
  %lncMZ = load i64, i64* %lsb6i
  %lncN0 = add i64 %lncMZ, 8
  %lncN2 = load i64*, i64** %Hp_Var
  %lncN3 = ptrtoint i64* %lncN2 to i64
  %lncN4 = add i64 %lncN3, -23
  %lncN5 = inttoptr i64 %lncN0 to i64*
  store i64 %lncN4, i64* %lncN5, !tbaa !5
  %lncN6 = ptrtoint i64* %Base_Arg to i64
  %lncN7 = inttoptr i64 %lncN6 to i8*
  %lncN8 = load i64, i64* %lsb6i
  %lncN9 = inttoptr i64 %lncN8 to i8*
  %lncNa = bitcast i8* @dirty_MUT_VAR to void (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  call ccc void (i8*, i8*) %lncNa( i8* %lncN7, i8* %lncN9 ) nounwind
  %lncNb = ptrtoint i8* @ghczmprim_GHCziTuple_Z0T_closure to i64
  %lncNc = add i64 %lncNb, 1
  store i64 %lncNc, i64* %R1_Var
  %lncNd = load i64*, i64** %Sp_Var
  %lncNe = getelementptr inbounds i64, i64* %lncNd, i32 2
  %lncNf = ptrtoint i64* %lncNe to i64
  %lncNg = inttoptr i64 %lncNf to i64*
  store i64* %lncNg, i64** %Sp_Var
  %lncNh = load i64*, i64** %Sp_Var
  %lncNi = getelementptr inbounds i64, i64* %lncNh, i32 0
  %lncNj = bitcast i64* %lncNi to i64*
  %lncNk = load i64, i64* %lncNj, !tbaa !1
  %lncNl = inttoptr i64 %lncNk to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncNm = load i64*, i64** %Sp_Var
  %lncNn = load i64*, i64** %Hp_Var
  %lncNo = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncNl( i64* %Base_Arg, i64* %lncNm, i64* %lncNn, i64 %lncNo, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccL1:
  %lncNp = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 32, i64* %lncNp, !tbaa !4
  %lncNq = load i64, i64* %R1_Var
  store i64 %lncNq, i64* %R1_Var
  %lncNr = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncNs = load i64*, i64** %Sp_Var
  %lncNt = load i64*, i64** %Hp_Var
  %lncNu = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncNr( i64* %Base_Arg, i64* %lncNs, i64* %lncNt, i64 %lncNu, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_bumpSync_closure_struct = type <{i64}>
@Disk_bumpSync_closure$def = internal global %Disk_bumpSync_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_bumpSync_info$def to i64)}>
@Disk_bumpSync_closure = alias i8* bitcast (%Disk_bumpSync_closure_struct* @Disk_bumpSync_closure$def to i8*)
@Disk_bumpSync_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_bumpSync_info$def to i8*)
define ghccc void @Disk_bumpSync_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934597, i64 0, i64 15}>
{
ccNA:
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncND = load i64, i64* %R2_Var
  store i64 %lncND, i64* %R2_Var
  %lncNE = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_bumpSync1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncNF = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncNE( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lncNF, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_bumpWrite1_closure_struct = type <{i64}>
@Disk_bumpWrite1_closure$def = internal global %Disk_bumpWrite1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_bumpWrite1_info$def to i64)}>
@Disk_bumpWrite1_closure = alias i8* bitcast (%Disk_bumpWrite1_closure_struct* @Disk_bumpWrite1_closure$def to i8*)
@Disk_bumpWrite1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_bumpWrite1_info$def to i8*)
define ghccc void @Disk_bumpWrite1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934597, i64 0, i64 15}>
{
ccO2:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncOp = load i64*, i64** %Sp_Var
  %lncOq = getelementptr inbounds i64, i64* %lncOp, i32 -2
  %lncOr = ptrtoint i64* %lncOq to i64
  %lncOs = icmp ult i64 %lncOr, %SpLim_Arg
  br i1 %lncOs, label %ccO6, label %ccO7
ccO7:
  %lncOu = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccNZ_info$def to i64
  %lncOt = load i64*, i64** %Sp_Var
  %lncOv = getelementptr inbounds i64, i64* %lncOt, i32 -1
  store i64 %lncOu, i64* %lncOv, !tbaa !1
  %lncOw = load i64, i64* %R2_Var
  store i64 %lncOw, i64* %R1_Var
  %lncOx = load i64*, i64** %Sp_Var
  %lncOy = getelementptr inbounds i64, i64* %lncOx, i32 -1
  %lncOz = ptrtoint i64* %lncOy to i64
  %lncOA = inttoptr i64 %lncOz to i64*
  store i64* %lncOA, i64** %Sp_Var
  %lncOB = load i64, i64* %R1_Var
  %lncOC = and i64 %lncOB, 7
  %lncOD = icmp ne i64 %lncOC, 0
  br i1 %lncOD, label %ucOo, label %ccO0
ccO0:
  %lncOF = load i64, i64* %R1_Var
  %lncOG = inttoptr i64 %lncOF to i64*
  %lncOH = load i64, i64* %lncOG, !tbaa !3
  %lncOI = inttoptr i64 %lncOH to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncOJ = load i64*, i64** %Sp_Var
  %lncOK = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncOI( i64* %Base_Arg, i64* %lncOJ, i64* %Hp_Arg, i64 %lncOK, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucOo:
  %lncOL = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccNZ_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncOM = load i64*, i64** %Sp_Var
  %lncON = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncOL( i64* %Base_Arg, i64* %lncOM, i64* %Hp_Arg, i64 %lncON, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccO6:
  %lncOO = load i64, i64* %R2_Var
  store i64 %lncOO, i64* %R2_Var
  %lncOP = ptrtoint %Disk_bumpWrite1_closure_struct* @Disk_bumpWrite1_closure$def to i64
  store i64 %lncOP, i64* %R1_Var
  %lncOQ = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lncOR = bitcast i64* %lncOQ to i64*
  %lncOS = load i64, i64* %lncOR, !tbaa !4
  %lncOT = inttoptr i64 %lncOS to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncOU = load i64*, i64** %Sp_Var
  %lncOV = load i64, i64* %R1_Var
  %lncOW = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncOT( i64* %Base_Arg, i64* %lncOU, i64* %Hp_Arg, i64 %lncOV, i64 %lncOW, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccNZ_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccNZ_info$def to i8*)
define internal ghccc void @ccNZ_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
ccNZ:
  %lsb6w = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncOY = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccO5_info$def to i64
  %lncOX = load i64*, i64** %Sp_Var
  %lncOZ = getelementptr inbounds i64, i64* %lncOX, i32 -1
  store i64 %lncOY, i64* %lncOZ, !tbaa !1
  %lncP2 = load i64, i64* %R1_Var
  %lncP3 = add i64 %lncP2, 7
  %lncP4 = inttoptr i64 %lncP3 to i64*
  %lncP5 = load i64, i64* %lncP4, !tbaa !3
  store i64 %lncP5, i64* %lsb6w
  %lncP6 = load i64, i64* %lsb6w
  %lncP7 = add i64 %lncP6, 8
  %lncP8 = inttoptr i64 %lncP7 to i64*
  %lncP9 = load i64, i64* %lncP8, !tbaa !5
  store i64 %lncP9, i64* %R1_Var
  %lncPb = load i64, i64* %lsb6w
  %lncPa = load i64*, i64** %Sp_Var
  %lncPc = getelementptr inbounds i64, i64* %lncPa, i32 0
  store i64 %lncPb, i64* %lncPc, !tbaa !1
  %lncPd = load i64*, i64** %Sp_Var
  %lncPe = getelementptr inbounds i64, i64* %lncPd, i32 -1
  %lncPf = ptrtoint i64* %lncPe to i64
  %lncPg = inttoptr i64 %lncPf to i64*
  store i64* %lncPg, i64** %Sp_Var
  %lncPh = load i64, i64* %R1_Var
  %lncPi = and i64 %lncPh, 7
  %lncPj = icmp ne i64 %lncPi, 0
  br i1 %lncPj, label %ucOn, label %ccO9
ccO9:
  %lncPl = load i64, i64* %R1_Var
  %lncPm = inttoptr i64 %lncPl to i64*
  %lncPn = load i64, i64* %lncPm, !tbaa !3
  %lncPo = inttoptr i64 %lncPn to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncPp = load i64*, i64** %Sp_Var
  %lncPq = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncPo( i64* %Base_Arg, i64* %lncPp, i64* %Hp_Arg, i64 %lncPq, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucOn:
  %lncPr = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccO5_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncPs = load i64*, i64** %Sp_Var
  %lncPt = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncPr( i64* %Base_Arg, i64* %lncPs, i64* %Hp_Arg, i64 %lncPt, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccO5_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccO5_info$def to i8*)
define internal ghccc void @ccO5_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 1, i64 32}>
{
ccO5:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb6B = alloca i64, i32 1
  %lsb6D = alloca i64, i32 1
  %lsb6E = alloca i64, i32 1
  %lsb6w = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncPu = load i64*, i64** %Hp_Var
  %lncPv = getelementptr inbounds i64, i64* %lncPu, i32 4
  %lncPw = ptrtoint i64* %lncPv to i64
  %lncPx = inttoptr i64 %lncPw to i64*
  store i64* %lncPx, i64** %Hp_Var
  %lncPy = load i64*, i64** %Hp_Var
  %lncPz = ptrtoint i64* %lncPy to i64
  %lncPA = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lncPB = bitcast i64* %lncPA to i64*
  %lncPC = load i64, i64* %lncPB, !tbaa !4
  %lncPD = icmp ugt i64 %lncPz, %lncPC
  br i1 %lncPD, label %ccOf, label %ccOe
ccOe:
  %lncPG = load i64, i64* %R1_Var
  %lncPH = add i64 %lncPG, 7
  %lncPI = inttoptr i64 %lncPH to i64*
  %lncPJ = load i64, i64* %lncPI, !tbaa !3
  store i64 %lncPJ, i64* %lsb6B
  %lncPM = load i64, i64* %R1_Var
  %lncPN = add i64 %lncPM, 23
  %lncPO = inttoptr i64 %lncPN to i64*
  %lncPP = load i64, i64* %lncPO, !tbaa !3
  store i64 %lncPP, i64* %lsb6D
  %lncPS = load i64, i64* %R1_Var
  %lncPT = add i64 %lncPS, 15
  %lncPU = inttoptr i64 %lncPT to i64*
  %lncPV = load i64, i64* %lncPU, !tbaa !3
  %lncPW = add i64 %lncPV, 1
  store i64 %lncPW, i64* %lsb6E
  %lncPY = ptrtoint i8* @Disk_Stats_con_info to i64
  %lncPX = load i64*, i64** %Hp_Var
  %lncPZ = getelementptr inbounds i64, i64* %lncPX, i32 -3
  store i64 %lncPY, i64* %lncPZ, !tbaa !2
  %lncQ1 = load i64, i64* %lsb6B
  %lncQ0 = load i64*, i64** %Hp_Var
  %lncQ2 = getelementptr inbounds i64, i64* %lncQ0, i32 -2
  store i64 %lncQ1, i64* %lncQ2, !tbaa !2
  %lncQ4 = load i64, i64* %lsb6E
  %lncQ3 = load i64*, i64** %Hp_Var
  %lncQ5 = getelementptr inbounds i64, i64* %lncQ3, i32 -1
  store i64 %lncQ4, i64* %lncQ5, !tbaa !2
  %lncQ7 = load i64, i64* %lsb6D
  %lncQ6 = load i64*, i64** %Hp_Var
  %lncQ8 = getelementptr inbounds i64, i64* %lncQ6, i32 0
  store i64 %lncQ7, i64* %lncQ8, !tbaa !2
  %lncQ9 = load i64*, i64** %Sp_Var
  %lncQa = getelementptr inbounds i64, i64* %lncQ9, i32 1
  %lncQb = bitcast i64* %lncQa to i64*
  %lncQc = load i64, i64* %lncQb, !tbaa !1
  store i64 %lncQc, i64* %lsb6w
  %lncQd = load i64, i64* %lsb6w
  %lncQe = add i64 %lncQd, 8
  %lncQg = load i64*, i64** %Hp_Var
  %lncQh = ptrtoint i64* %lncQg to i64
  %lncQi = add i64 %lncQh, -23
  %lncQj = inttoptr i64 %lncQe to i64*
  store i64 %lncQi, i64* %lncQj, !tbaa !5
  %lncQk = ptrtoint i64* %Base_Arg to i64
  %lncQl = inttoptr i64 %lncQk to i8*
  %lncQm = load i64, i64* %lsb6w
  %lncQn = inttoptr i64 %lncQm to i8*
  %lncQo = bitcast i8* @dirty_MUT_VAR to void (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  call ccc void (i8*, i8*) %lncQo( i8* %lncQl, i8* %lncQn ) nounwind
  %lncQp = ptrtoint i8* @ghczmprim_GHCziTuple_Z0T_closure to i64
  %lncQq = add i64 %lncQp, 1
  store i64 %lncQq, i64* %R1_Var
  %lncQr = load i64*, i64** %Sp_Var
  %lncQs = getelementptr inbounds i64, i64* %lncQr, i32 2
  %lncQt = ptrtoint i64* %lncQs to i64
  %lncQu = inttoptr i64 %lncQt to i64*
  store i64* %lncQu, i64** %Sp_Var
  %lncQv = load i64*, i64** %Sp_Var
  %lncQw = getelementptr inbounds i64, i64* %lncQv, i32 0
  %lncQx = bitcast i64* %lncQw to i64*
  %lncQy = load i64, i64* %lncQx, !tbaa !1
  %lncQz = inttoptr i64 %lncQy to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncQA = load i64*, i64** %Sp_Var
  %lncQB = load i64*, i64** %Hp_Var
  %lncQC = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncQz( i64* %Base_Arg, i64* %lncQA, i64* %lncQB, i64 %lncQC, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccOf:
  %lncQD = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 32, i64* %lncQD, !tbaa !4
  %lncQE = load i64, i64* %R1_Var
  store i64 %lncQE, i64* %R1_Var
  %lncQF = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncQG = load i64*, i64** %Sp_Var
  %lncQH = load i64*, i64** %Hp_Var
  %lncQI = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncQF( i64* %Base_Arg, i64* %lncQG, i64* %lncQH, i64 %lncQI, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_bumpWrite_closure_struct = type <{i64}>
@Disk_bumpWrite_closure$def = internal global %Disk_bumpWrite_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_bumpWrite_info$def to i64)}>
@Disk_bumpWrite_closure = alias i8* bitcast (%Disk_bumpWrite_closure_struct* @Disk_bumpWrite_closure$def to i8*)
@Disk_bumpWrite_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_bumpWrite_info$def to i8*)
define ghccc void @Disk_bumpWrite_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934597, i64 0, i64 15}>
{
ccQO:
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncQR = load i64, i64* %R2_Var
  store i64 %lncQR, i64* %R2_Var
  %lncQS = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_bumpWrite1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncQT = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncQS( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lncQT, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_bumpRead1_closure_struct = type <{i64}>
@Disk_bumpRead1_closure$def = internal global %Disk_bumpRead1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_bumpRead1_info$def to i64)}>
@Disk_bumpRead1_closure = alias i8* bitcast (%Disk_bumpRead1_closure_struct* @Disk_bumpRead1_closure$def to i8*)
@Disk_bumpRead1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_bumpRead1_info$def to i8*)
define ghccc void @Disk_bumpRead1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934597, i64 0, i64 15}>
{
ccRg:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncRD = load i64*, i64** %Sp_Var
  %lncRE = getelementptr inbounds i64, i64* %lncRD, i32 -2
  %lncRF = ptrtoint i64* %lncRE to i64
  %lncRG = icmp ult i64 %lncRF, %SpLim_Arg
  br i1 %lncRG, label %ccRk, label %ccRl
ccRl:
  %lncRI = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccRd_info$def to i64
  %lncRH = load i64*, i64** %Sp_Var
  %lncRJ = getelementptr inbounds i64, i64* %lncRH, i32 -1
  store i64 %lncRI, i64* %lncRJ, !tbaa !1
  %lncRK = load i64, i64* %R2_Var
  store i64 %lncRK, i64* %R1_Var
  %lncRL = load i64*, i64** %Sp_Var
  %lncRM = getelementptr inbounds i64, i64* %lncRL, i32 -1
  %lncRN = ptrtoint i64* %lncRM to i64
  %lncRO = inttoptr i64 %lncRN to i64*
  store i64* %lncRO, i64** %Sp_Var
  %lncRP = load i64, i64* %R1_Var
  %lncRQ = and i64 %lncRP, 7
  %lncRR = icmp ne i64 %lncRQ, 0
  br i1 %lncRR, label %ucRC, label %ccRe
ccRe:
  %lncRT = load i64, i64* %R1_Var
  %lncRU = inttoptr i64 %lncRT to i64*
  %lncRV = load i64, i64* %lncRU, !tbaa !3
  %lncRW = inttoptr i64 %lncRV to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncRX = load i64*, i64** %Sp_Var
  %lncRY = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncRW( i64* %Base_Arg, i64* %lncRX, i64* %Hp_Arg, i64 %lncRY, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucRC:
  %lncRZ = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccRd_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncS0 = load i64*, i64** %Sp_Var
  %lncS1 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncRZ( i64* %Base_Arg, i64* %lncS0, i64* %Hp_Arg, i64 %lncS1, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccRk:
  %lncS2 = load i64, i64* %R2_Var
  store i64 %lncS2, i64* %R2_Var
  %lncS3 = ptrtoint %Disk_bumpRead1_closure_struct* @Disk_bumpRead1_closure$def to i64
  store i64 %lncS3, i64* %R1_Var
  %lncS4 = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lncS5 = bitcast i64* %lncS4 to i64*
  %lncS6 = load i64, i64* %lncS5, !tbaa !4
  %lncS7 = inttoptr i64 %lncS6 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncS8 = load i64*, i64** %Sp_Var
  %lncS9 = load i64, i64* %R1_Var
  %lncSa = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncS7( i64* %Base_Arg, i64* %lncS8, i64* %Hp_Arg, i64 %lncS9, i64 %lncSa, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccRd_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccRd_info$def to i8*)
define internal ghccc void @ccRd_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
ccRd:
  %lsb6K = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncSc = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccRj_info$def to i64
  %lncSb = load i64*, i64** %Sp_Var
  %lncSd = getelementptr inbounds i64, i64* %lncSb, i32 -1
  store i64 %lncSc, i64* %lncSd, !tbaa !1
  %lncSg = load i64, i64* %R1_Var
  %lncSh = add i64 %lncSg, 7
  %lncSi = inttoptr i64 %lncSh to i64*
  %lncSj = load i64, i64* %lncSi, !tbaa !3
  store i64 %lncSj, i64* %lsb6K
  %lncSk = load i64, i64* %lsb6K
  %lncSl = add i64 %lncSk, 8
  %lncSm = inttoptr i64 %lncSl to i64*
  %lncSn = load i64, i64* %lncSm, !tbaa !5
  store i64 %lncSn, i64* %R1_Var
  %lncSp = load i64, i64* %lsb6K
  %lncSo = load i64*, i64** %Sp_Var
  %lncSq = getelementptr inbounds i64, i64* %lncSo, i32 0
  store i64 %lncSp, i64* %lncSq, !tbaa !1
  %lncSr = load i64*, i64** %Sp_Var
  %lncSs = getelementptr inbounds i64, i64* %lncSr, i32 -1
  %lncSt = ptrtoint i64* %lncSs to i64
  %lncSu = inttoptr i64 %lncSt to i64*
  store i64* %lncSu, i64** %Sp_Var
  %lncSv = load i64, i64* %R1_Var
  %lncSw = and i64 %lncSv, 7
  %lncSx = icmp ne i64 %lncSw, 0
  br i1 %lncSx, label %ucRB, label %ccRn
ccRn:
  %lncSz = load i64, i64* %R1_Var
  %lncSA = inttoptr i64 %lncSz to i64*
  %lncSB = load i64, i64* %lncSA, !tbaa !3
  %lncSC = inttoptr i64 %lncSB to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncSD = load i64*, i64** %Sp_Var
  %lncSE = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncSC( i64* %Base_Arg, i64* %lncSD, i64* %Hp_Arg, i64 %lncSE, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucRB:
  %lncSF = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccRj_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncSG = load i64*, i64** %Sp_Var
  %lncSH = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncSF( i64* %Base_Arg, i64* %lncSG, i64* %Hp_Arg, i64 %lncSH, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccRj_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccRj_info$def to i8*)
define internal ghccc void @ccRj_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 1, i64 32}>
{
ccRj:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb6Q = alloca i64, i32 1
  %lsb6R = alloca i64, i32 1
  %lsb6S = alloca i64, i32 1
  %lsb6K = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncSI = load i64*, i64** %Hp_Var
  %lncSJ = getelementptr inbounds i64, i64* %lncSI, i32 4
  %lncSK = ptrtoint i64* %lncSJ to i64
  %lncSL = inttoptr i64 %lncSK to i64*
  store i64* %lncSL, i64** %Hp_Var
  %lncSM = load i64*, i64** %Hp_Var
  %lncSN = ptrtoint i64* %lncSM to i64
  %lncSO = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lncSP = bitcast i64* %lncSO to i64*
  %lncSQ = load i64, i64* %lncSP, !tbaa !4
  %lncSR = icmp ugt i64 %lncSN, %lncSQ
  br i1 %lncSR, label %ccRt, label %ccRs
ccRs:
  %lncSU = load i64, i64* %R1_Var
  %lncSV = add i64 %lncSU, 15
  %lncSW = inttoptr i64 %lncSV to i64*
  %lncSX = load i64, i64* %lncSW, !tbaa !3
  store i64 %lncSX, i64* %lsb6Q
  %lncT0 = load i64, i64* %R1_Var
  %lncT1 = add i64 %lncT0, 23
  %lncT2 = inttoptr i64 %lncT1 to i64*
  %lncT3 = load i64, i64* %lncT2, !tbaa !3
  store i64 %lncT3, i64* %lsb6R
  %lncT6 = load i64, i64* %R1_Var
  %lncT7 = add i64 %lncT6, 7
  %lncT8 = inttoptr i64 %lncT7 to i64*
  %lncT9 = load i64, i64* %lncT8, !tbaa !3
  %lncTa = add i64 %lncT9, 1
  store i64 %lncTa, i64* %lsb6S
  %lncTc = ptrtoint i8* @Disk_Stats_con_info to i64
  %lncTb = load i64*, i64** %Hp_Var
  %lncTd = getelementptr inbounds i64, i64* %lncTb, i32 -3
  store i64 %lncTc, i64* %lncTd, !tbaa !2
  %lncTf = load i64, i64* %lsb6S
  %lncTe = load i64*, i64** %Hp_Var
  %lncTg = getelementptr inbounds i64, i64* %lncTe, i32 -2
  store i64 %lncTf, i64* %lncTg, !tbaa !2
  %lncTi = load i64, i64* %lsb6Q
  %lncTh = load i64*, i64** %Hp_Var
  %lncTj = getelementptr inbounds i64, i64* %lncTh, i32 -1
  store i64 %lncTi, i64* %lncTj, !tbaa !2
  %lncTl = load i64, i64* %lsb6R
  %lncTk = load i64*, i64** %Hp_Var
  %lncTm = getelementptr inbounds i64, i64* %lncTk, i32 0
  store i64 %lncTl, i64* %lncTm, !tbaa !2
  %lncTn = load i64*, i64** %Sp_Var
  %lncTo = getelementptr inbounds i64, i64* %lncTn, i32 1
  %lncTp = bitcast i64* %lncTo to i64*
  %lncTq = load i64, i64* %lncTp, !tbaa !1
  store i64 %lncTq, i64* %lsb6K
  %lncTr = load i64, i64* %lsb6K
  %lncTs = add i64 %lncTr, 8
  %lncTu = load i64*, i64** %Hp_Var
  %lncTv = ptrtoint i64* %lncTu to i64
  %lncTw = add i64 %lncTv, -23
  %lncTx = inttoptr i64 %lncTs to i64*
  store i64 %lncTw, i64* %lncTx, !tbaa !5
  %lncTy = ptrtoint i64* %Base_Arg to i64
  %lncTz = inttoptr i64 %lncTy to i8*
  %lncTA = load i64, i64* %lsb6K
  %lncTB = inttoptr i64 %lncTA to i8*
  %lncTC = bitcast i8* @dirty_MUT_VAR to void (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  call ccc void (i8*, i8*) %lncTC( i8* %lncTz, i8* %lncTB ) nounwind
  %lncTD = ptrtoint i8* @ghczmprim_GHCziTuple_Z0T_closure to i64
  %lncTE = add i64 %lncTD, 1
  store i64 %lncTE, i64* %R1_Var
  %lncTF = load i64*, i64** %Sp_Var
  %lncTG = getelementptr inbounds i64, i64* %lncTF, i32 2
  %lncTH = ptrtoint i64* %lncTG to i64
  %lncTI = inttoptr i64 %lncTH to i64*
  store i64* %lncTI, i64** %Sp_Var
  %lncTJ = load i64*, i64** %Sp_Var
  %lncTK = getelementptr inbounds i64, i64* %lncTJ, i32 0
  %lncTL = bitcast i64* %lncTK to i64*
  %lncTM = load i64, i64* %lncTL, !tbaa !1
  %lncTN = inttoptr i64 %lncTM to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncTO = load i64*, i64** %Sp_Var
  %lncTP = load i64*, i64** %Hp_Var
  %lncTQ = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncTN( i64* %Base_Arg, i64* %lncTO, i64* %lncTP, i64 %lncTQ, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccRt:
  %lncTR = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 32, i64* %lncTR, !tbaa !4
  %lncTS = load i64, i64* %R1_Var
  store i64 %lncTS, i64* %R1_Var
  %lncTT = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncTU = load i64*, i64** %Sp_Var
  %lncTV = load i64*, i64** %Hp_Var
  %lncTW = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncTT( i64* %Base_Arg, i64* %lncTU, i64* %lncTV, i64 %lncTW, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_bumpRead_closure_struct = type <{i64}>
@Disk_bumpRead_closure$def = internal global %Disk_bumpRead_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_bumpRead_info$def to i64)}>
@Disk_bumpRead_closure = alias i8* bitcast (%Disk_bumpRead_closure_struct* @Disk_bumpRead_closure$def to i8*)
@Disk_bumpRead_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_bumpRead_info$def to i8*)
define ghccc void @Disk_bumpRead_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934597, i64 0, i64 15}>
{
ccU2:
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncU5 = load i64, i64* %R2_Var
  store i64 %lncU5, i64* %R2_Var
  %lncU6 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_bumpRead1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncU7 = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncU6( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lncU7, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_debugmsg2_closure_struct = type <{i64}>
@Disk_debugmsg2_closure$def = internal global %Disk_debugmsg2_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_debugmsg2_info$def to i64)}>
@Disk_debugmsg2_closure = alias i8* bitcast (%Disk_debugmsg2_closure_struct* @Disk_debugmsg2_closure$def to i8*)
@Disk_debugmsg2_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_debugmsg2_info$def to i8*)
define ghccc void @Disk_debugmsg2_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 4294967299, i64 0, i64 15}>
{
ccUd:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lncUg = ptrtoint i8* @ghczmprim_GHCziTuple_Z0T_closure to i64
  %lncUh = add i64 %lncUg, 1
  store i64 %lncUh, i64* %R1_Var
  %lncUi = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  %lncUj = bitcast i64* %lncUi to i64*
  %lncUk = load i64, i64* %lncUj, !tbaa !1
  %lncUl = inttoptr i64 %lncUk to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncUm = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncUl( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lncUm, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_debugmsg1_closure_struct = type <{i64}>
@Disk_debugmsg1_closure$def = internal global %Disk_debugmsg1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_debugmsg1_info$def to i64)}>
@Disk_debugmsg1_closure = alias i8* bitcast (%Disk_debugmsg1_closure_struct* @Disk_debugmsg1_closure$def to i8*)
@Disk_debugmsg1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_debugmsg1_info$def to i8*)
define ghccc void @Disk_debugmsg1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934597, i64 0, i64 15}>
{
ccUs:
  %lncUv = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_debugmsg2_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncUv( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_debugmsg_closure_struct = type <{i64}>
@Disk_debugmsg_closure$def = internal global %Disk_debugmsg_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_debugmsg_info$def to i64)}>
@Disk_debugmsg_closure = alias i8* bitcast (%Disk_debugmsg_closure_struct* @Disk_debugmsg_closure$def to i8*)
@Disk_debugmsg_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_debugmsg_info$def to i8*)
define ghccc void @Disk_debugmsg_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934597, i64 0, i64 15}>
{
ccUB:
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncUE = load i64, i64* %R2_Var
  store i64 %lncUE, i64* %R2_Var
  %lncUF = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_debugmsg1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncUG = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncUF( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lncUG, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_readzudisk4_closure_struct = type <{i64, i64}>
@Disk_readzudisk4_closure$def = internal global %Disk_readzudisk4_closure_struct<{i64 ptrtoint (i8* @integerzmgmp_GHCziIntegerziType_Szh_static_info to i64), i64 4096}>
@Disk_readzudisk4_closure = alias i8* bitcast (%Disk_readzudisk4_closure_struct* @Disk_readzudisk4_closure$def to i8*)
%Disk_readzudisk3_closure_struct = type <{i64, i64}>
%ccVD_str_struct = type <{[4 x i8]}>
%ccWl_str_struct = type <{[24 x i8]}>
@Disk_readzudisk3_closure$def = internal global %Disk_readzudisk3_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_readzudisk3_info$def to i64), i64 0}>
@Disk_readzudisk3_closure = alias i8* bitcast (%Disk_readzudisk3_closure_struct* @Disk_readzudisk3_closure$def to i8*)
@ccVD_str$def = internal constant %ccVD_str_struct<{[4 x i8] [i8 32, i8 64, i8 32, i8 0]}>
@ccVD_str = internal alias i8* bitcast (%ccVD_str_struct* @ccVD_str$def to i8*)
@ccWl_str$def = internal constant %ccWl_str_struct<{[24 x i8] [i8 114, i8 101, i8 97, i8 100, i8 95, i8 100, i8 105, i8 115, i8 107, i8 58, i8 32, i8 115, i8 104, i8 111, i8 114, i8 116, i8 32, i8 114, i8 101, i8 97, i8 100, i8 58, i8 32, i8 0]}>
@ccWl_str = internal alias i8* bitcast (%ccWl_str_struct* @ccWl_str$def to i8*)
@sb77_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb77_info$def to i8*)
define internal ghccc void @sb77_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb77_info$def to i64)),i64 240), i64 1, i64 4294967313}>
{
ccVx:
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lncWw = load i64*, i64** %Sp_Var
  %lncWx = getelementptr inbounds i64, i64* %lncWw, i32 -3
  %lncWy = ptrtoint i64* %lncWx to i64
  %lncWz = icmp ult i64 %lncWy, %SpLim_Arg
  br i1 %lncWz, label %ccVy, label %ccVz
ccVz:
  %lncWB = ptrtoint i8* @stg_upd_frame_info to i64
  %lncWA = load i64*, i64** %Sp_Var
  %lncWC = getelementptr inbounds i64, i64* %lncWA, i32 -2
  store i64 %lncWB, i64* %lncWC, !tbaa !1
  %lncWE = load i64, i64* %R1_Var
  %lncWD = load i64*, i64** %Sp_Var
  %lncWF = getelementptr inbounds i64, i64* %lncWD, i32 -1
  store i64 %lncWE, i64* %lncWF, !tbaa !1
  %lncWH = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccVu_info$def to i64
  %lncWG = load i64*, i64** %Sp_Var
  %lncWI = getelementptr inbounds i64, i64* %lncWG, i32 -3
  store i64 %lncWH, i64* %lncWI, !tbaa !1
  %lncWJ = ptrtoint i8* @ghczmprim_GHCziTypes_ZMZN_closure to i64
  %lncWK = add i64 %lncWJ, 1
  store i64 %lncWK, i64* %R4_Var
  %lncWN = load i64, i64* %R1_Var
  %lncWO = add i64 %lncWN, 16
  %lncWP = inttoptr i64 %lncWO to i64*
  %lncWQ = load i64, i64* %lncWP, !tbaa !3
  store i64 %lncWQ, i64* %R3_Var
  store i64 0, i64* %R2_Var
  %lncWR = load i64*, i64** %Sp_Var
  %lncWS = getelementptr inbounds i64, i64* %lncWR, i32 -3
  %lncWT = ptrtoint i64* %lncWS to i64
  %lncWU = inttoptr i64 %lncWT to i64*
  store i64* %lncWU, i64** %Sp_Var
  %lncWV = bitcast i8* @base_GHCziShow_zdwzdcshowsPrec3_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncWW = load i64*, i64** %Sp_Var
  %lncWX = load i64, i64* %R1_Var
  %lncWY = load i64, i64* %R2_Var
  %lncWZ = load i64, i64* %R3_Var
  %lncX0 = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncWV( i64* %Base_Arg, i64* %lncWW, i64* %Hp_Arg, i64 %lncWX, i64 %lncWY, i64 %lncWZ, i64 %lncX0, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccVy:
  %lncX1 = load i64, i64* %R1_Var
  store i64 %lncX1, i64* %R1_Var
  %lncX2 = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lncX3 = bitcast i64* %lncX2 to i64*
  %lncX4 = load i64, i64* %lncX3, !tbaa !4
  %lncX5 = inttoptr i64 %lncX4 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncX6 = load i64*, i64** %Sp_Var
  %lncX7 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncX5( i64* %Base_Arg, i64* %lncX6, i64* %Hp_Arg, i64 %lncX7, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccVu_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccVu_info$def to i8*)
define internal ghccc void @ccVu_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
ccVu:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lncX8 = load i64*, i64** %Hp_Var
  %lncX9 = getelementptr inbounds i64, i64* %lncX8, i32 3
  %lncXa = ptrtoint i64* %lncX9 to i64
  %lncXb = inttoptr i64 %lncXa to i64*
  store i64* %lncXb, i64** %Hp_Var
  %lncXc = load i64*, i64** %Hp_Var
  %lncXd = ptrtoint i64* %lncXc to i64
  %lncXe = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lncXf = bitcast i64* %lncXe to i64*
  %lncXg = load i64, i64* %lncXf, !tbaa !4
  %lncXh = icmp ugt i64 %lncXd, %lncXg
  br i1 %lncXh, label %ccVC, label %ccVB
ccVB:
  %lncXj = ptrtoint i8* @ghczmprim_GHCziTypes_ZC_con_info to i64
  %lncXi = load i64*, i64** %Hp_Var
  %lncXk = getelementptr inbounds i64, i64* %lncXi, i32 -2
  store i64 %lncXj, i64* %lncXk, !tbaa !2
  %lncXm = load i64, i64* %R1_Var
  %lncXl = load i64*, i64** %Hp_Var
  %lncXn = getelementptr inbounds i64, i64* %lncXl, i32 -1
  store i64 %lncXm, i64* %lncXn, !tbaa !2
  %lncXp = load i64, i64* %R2_Var
  %lncXo = load i64*, i64** %Hp_Var
  %lncXq = getelementptr inbounds i64, i64* %lncXo, i32 0
  store i64 %lncXp, i64* %lncXq, !tbaa !2
  %lncXs = load i64*, i64** %Hp_Var
  %lncXt = ptrtoint i64* %lncXs to i64
  %lncXu = add i64 %lncXt, -14
  store i64 %lncXu, i64* %R1_Var
  %lncXv = load i64*, i64** %Sp_Var
  %lncXw = getelementptr inbounds i64, i64* %lncXv, i32 1
  %lncXx = ptrtoint i64* %lncXw to i64
  %lncXy = inttoptr i64 %lncXx to i64*
  store i64* %lncXy, i64** %Sp_Var
  %lncXz = load i64*, i64** %Sp_Var
  %lncXA = getelementptr inbounds i64, i64* %lncXz, i32 0
  %lncXB = bitcast i64* %lncXA to i64*
  %lncXC = load i64, i64* %lncXB, !tbaa !1
  %lncXD = inttoptr i64 %lncXC to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncXE = load i64*, i64** %Sp_Var
  %lncXF = load i64*, i64** %Hp_Var
  %lncXG = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncXD( i64* %Base_Arg, i64* %lncXE, i64* %lncXF, i64 %lncXG, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccVC:
  %lncXH = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lncXH, !tbaa !4
  %lncXI = load i64, i64* %R2_Var
  store i64 %lncXI, i64* %R2_Var
  %lncXJ = load i64, i64* %R1_Var
  store i64 %lncXJ, i64* %R1_Var
  %lncXK = bitcast i8* @stg_gc_pp to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncXL = load i64*, i64** %Sp_Var
  %lncXM = load i64*, i64** %Hp_Var
  %lncXN = load i64, i64* %R1_Var
  %lncXO = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncXK( i64* %Base_Arg, i64* %lncXL, i64* %lncXM, i64 %lncXN, i64 %lncXO, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@sb78_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb78_info$def to i8*)
define internal ghccc void @sb78_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb78_info$def to i64)),i64 240), i64 1, i64 4294967313}>
{
ccVE:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb6X = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lncXP = load i64*, i64** %Hp_Var
  %lncXQ = getelementptr inbounds i64, i64* %lncXP, i32 3
  %lncXR = ptrtoint i64* %lncXQ to i64
  %lncXS = inttoptr i64 %lncXR to i64*
  store i64* %lncXS, i64** %Hp_Var
  %lncXT = load i64*, i64** %Hp_Var
  %lncXU = ptrtoint i64* %lncXT to i64
  %lncXV = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lncXW = bitcast i64* %lncXV to i64*
  %lncXX = load i64, i64* %lncXW, !tbaa !4
  %lncXY = icmp ugt i64 %lncXU, %lncXX
  br i1 %lncXY, label %ccVI, label %ccVH
ccVH:
  %lncY1 = load i64, i64* %R1_Var
  %lncY2 = add i64 %lncY1, 16
  %lncY3 = inttoptr i64 %lncY2 to i64*
  %lncY4 = load i64, i64* %lncY3, !tbaa !3
  store i64 %lncY4, i64* %lsb6X
  %lncY6 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb77_info$def to i64
  %lncY5 = load i64*, i64** %Hp_Var
  %lncY7 = getelementptr inbounds i64, i64* %lncY5, i32 -2
  store i64 %lncY6, i64* %lncY7, !tbaa !2
  %lncY9 = load i64, i64* %lsb6X
  %lncY8 = load i64*, i64** %Hp_Var
  %lncYa = getelementptr inbounds i64, i64* %lncY8, i32 0
  store i64 %lncY9, i64* %lncYa, !tbaa !2
  %lncYb = load i64*, i64** %Hp_Var
  %lncYc = getelementptr inbounds i64, i64* %lncYb, i32 -2
  %lncYd = ptrtoint i64* %lncYc to i64
  store i64 %lncYd, i64* %R3_Var
  %lncYe = ptrtoint %ccVD_str_struct* @ccVD_str$def to i64
  store i64 %lncYe, i64* %R2_Var
  %lncYf = bitcast i8* @ghczmprim_GHCziCString_unpackAppendCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncYg = load i64*, i64** %Hp_Var
  %lncYh = load i64, i64* %R1_Var
  %lncYi = load i64, i64* %R2_Var
  %lncYj = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncYf( i64* %Base_Arg, i64* %Sp_Arg, i64* %lncYg, i64 %lncYh, i64 %lncYi, i64 %lncYj, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccVI:
  %lncYk = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lncYk, !tbaa !4
  %lncYl = load i64, i64* %R1_Var
  store i64 %lncYl, i64* %R1_Var
  %lncYm = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lncYn = bitcast i64* %lncYm to i64*
  %lncYo = load i64, i64* %lncYn, !tbaa !4
  %lncYp = inttoptr i64 %lncYo to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncYq = load i64*, i64** %Hp_Var
  %lncYr = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncYp( i64* %Base_Arg, i64* %Sp_Arg, i64* %lncYq, i64 %lncYr, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@sb7j_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb7j_info$def to i8*)
define internal ghccc void @sb7j_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb7j_info$def to i64)),i64 240), i64 4294967297, i64 12884901908}>
{
ccVJ:
  %lsb6X = alloca i64, i32 1
  %lsb6Y = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lncYs = load i64*, i64** %Sp_Var
  %lncYt = getelementptr inbounds i64, i64* %lncYs, i32 -5
  %lncYu = ptrtoint i64* %lncYt to i64
  %lncYv = icmp ult i64 %lncYu, %SpLim_Arg
  br i1 %lncYv, label %ccVK, label %ccWe
ccWe:
  %lncYx = ptrtoint i8* @stg_upd_frame_info to i64
  %lncYw = load i64*, i64** %Sp_Var
  %lncYy = getelementptr inbounds i64, i64* %lncYw, i32 -2
  store i64 %lncYx, i64* %lncYy, !tbaa !1
  %lncYA = load i64, i64* %R1_Var
  %lncYz = load i64*, i64** %Sp_Var
  %lncYB = getelementptr inbounds i64, i64* %lncYz, i32 -1
  store i64 %lncYA, i64* %lncYB, !tbaa !1
  %lncYE = load i64, i64* %R1_Var
  %lncYF = add i64 %lncYE, 16
  %lncYG = inttoptr i64 %lncYF to i64*
  %lncYH = load i64, i64* %lncYG, !tbaa !3
  store i64 %lncYH, i64* %lsb6X
  %lncYK = load i64, i64* %R1_Var
  %lncYL = add i64 %lncYK, 24
  %lncYM = inttoptr i64 %lncYL to i64*
  %lncYN = load i64, i64* %lncYM, !tbaa !3
  store i64 %lncYN, i64* %lsb6Y
  %lncYO = load i64, i64* %lsb6Y
  %lncYP = icmp sge i64 %lncYO, 0
  %lncYQ = zext i1 %lncYP to i64
  switch i64 %lncYQ, label %ccVW [i64 0, label %ccVW
i64 1, label %ccW6]
ccVW:
  %lncYS = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccVU_info$def to i64
  %lncYR = load i64*, i64** %Sp_Var
  %lncYT = getelementptr inbounds i64, i64* %lncYR, i32 -4
  store i64 %lncYS, i64* %lncYT, !tbaa !1
  %lncYU = load i64, i64* %lsb6Y
  store i64 %lncYU, i64* %R2_Var
  %lncYW = load i64, i64* %lsb6X
  %lncYV = load i64*, i64** %Sp_Var
  %lncYX = getelementptr inbounds i64, i64* %lncYV, i32 -3
  store i64 %lncYW, i64* %lncYX, !tbaa !1
  %lncYY = load i64*, i64** %Sp_Var
  %lncYZ = getelementptr inbounds i64, i64* %lncYY, i32 -4
  %lncZ0 = ptrtoint i64* %lncYZ to i64
  %lncZ1 = inttoptr i64 %lncZ0 to i64*
  store i64* %lncZ1, i64** %Sp_Var
  %lncZ2 = bitcast i8* @integerzmgmp_GHCziIntegerziType_wordToInteger_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncZ3 = load i64*, i64** %Sp_Var
  %lncZ4 = load i64, i64* %R1_Var
  %lncZ5 = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncZ2( i64* %Base_Arg, i64* %lncZ3, i64* %Hp_Arg, i64 %lncZ4, i64 %lncZ5, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccW6:
  %lncZ7 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccW5_info$def to i64
  %lncZ6 = load i64*, i64** %Sp_Var
  %lncZ8 = getelementptr inbounds i64, i64* %lncZ6, i32 -4
  store i64 %lncZ7, i64* %lncZ8, !tbaa !1
  %lncZ9 = load i64, i64* %lsb6Y
  store i64 %lncZ9, i64* %R2_Var
  %lncZb = load i64, i64* %lsb6X
  %lncZa = load i64*, i64** %Sp_Var
  %lncZc = getelementptr inbounds i64, i64* %lncZa, i32 -3
  store i64 %lncZb, i64* %lncZc, !tbaa !1
  %lncZd = load i64*, i64** %Sp_Var
  %lncZe = getelementptr inbounds i64, i64* %lncZd, i32 -4
  %lncZf = ptrtoint i64* %lncZe to i64
  %lncZg = inttoptr i64 %lncZf to i64*
  store i64* %lncZg, i64** %Sp_Var
  %lncZh = bitcast i8* @integerzmgmp_GHCziIntegerziType_smallInteger_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncZi = load i64*, i64** %Sp_Var
  %lncZj = load i64, i64* %R1_Var
  %lncZk = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncZh( i64* %Base_Arg, i64* %lncZi, i64* %Hp_Arg, i64 %lncZj, i64 %lncZk, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccVK:
  %lncZl = load i64, i64* %R1_Var
  store i64 %lncZl, i64* %R1_Var
  %lncZm = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lncZn = bitcast i64* %lncZm to i64*
  %lncZo = load i64, i64* %lncZn, !tbaa !4
  %lncZp = inttoptr i64 %lncZo to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncZq = load i64*, i64** %Sp_Var
  %lncZr = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncZp( i64* %Base_Arg, i64* %lncZq, i64* %Hp_Arg, i64 %lncZr, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccW5_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccW5_info$def to i8*)
define internal ghccc void @ccW5_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccW5_info$def to i64)),i64 240), i64 1, i64 4294967328}>
{
ccW5:
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %lncZs = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccWa_info$def to i64
  %lncZt = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lncZs, i64* %lncZt, !tbaa !1
  %lncZu = ptrtoint i8* @ghczmprim_GHCziTypes_ZMZN_closure to i64
  %lncZv = add i64 %lncZu, 1
  store i64 %lncZv, i64* %R4_Var
  store i64 %R1_Arg, i64* %R3_Var
  store i64 0, i64* %R2_Var
  %lncZw = bitcast i8* @base_GHCziShow_zdwzdcshowsPrec3_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncZx = load i64, i64* %R2_Var
  %lncZy = load i64, i64* %R3_Var
  %lncZz = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncZw( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lncZx, i64 %lncZy, i64 %lncZz, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccWa_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccWa_info$def to i8*)
define internal ghccc void @ccWa_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccWa_info$def to i64)),i64 240), i64 1, i64 4294967328}>
{
ccWa:
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncZA = load i64*, i64** %Sp_Var
  %lncZB = getelementptr inbounds i64, i64* %lncZA, i32 -1
  store i64 %R2_Arg, i64* %lncZB, !tbaa !1
  %lncZC = load i64*, i64** %Sp_Var
  %lncZD = getelementptr inbounds i64, i64* %lncZC, i32 0
  store i64 %R1_Arg, i64* %lncZD, !tbaa !1
  %lncZE = load i64*, i64** %Sp_Var
  %lncZF = getelementptr inbounds i64, i64* %lncZE, i32 -1
  %lncZG = ptrtoint i64* %lncZF to i64
  %lncZH = inttoptr i64 %lncZG to i64*
  store i64* %lncZH, i64** %Sp_Var
  %lncZI = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccVk_entry$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncZJ = load i64*, i64** %Sp_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncZI( i64* %Base_Arg, i64* %lncZJ, i64* %Hp_Arg, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccVU_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccVU_info$def to i8*)
define internal ghccc void @ccVU_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccVU_info$def to i64)),i64 240), i64 1, i64 4294967328}>
{
ccVU:
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %lncZK = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccW0_info$def to i64
  %lncZL = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lncZK, i64* %lncZL, !tbaa !1
  %lncZM = ptrtoint i8* @ghczmprim_GHCziTypes_ZMZN_closure to i64
  %lncZN = add i64 %lncZM, 1
  store i64 %lncZN, i64* %R4_Var
  store i64 %R1_Arg, i64* %R3_Var
  store i64 0, i64* %R2_Var
  %lncZO = bitcast i8* @base_GHCziShow_zdwzdcshowsPrec3_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncZP = load i64, i64* %R2_Var
  %lncZQ = load i64, i64* %R3_Var
  %lncZR = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncZO( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lncZP, i64 %lncZQ, i64 %lncZR, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccW0_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccW0_info$def to i8*)
define internal ghccc void @ccW0_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccW0_info$def to i64)),i64 240), i64 1, i64 4294967328}>
{
ccW0:
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncZS = load i64*, i64** %Sp_Var
  %lncZT = getelementptr inbounds i64, i64* %lncZS, i32 0
  store i64 %R1_Arg, i64* %lncZT, !tbaa !1
  %lncZU = load i64*, i64** %Sp_Var
  %lncZV = getelementptr inbounds i64, i64* %lncZU, i32 -1
  store i64 %R2_Arg, i64* %lncZV, !tbaa !1
  %lncZW = load i64*, i64** %Sp_Var
  %lncZX = getelementptr inbounds i64, i64* %lncZW, i32 -1
  %lncZY = ptrtoint i64* %lncZX to i64
  %lncZZ = inttoptr i64 %lncZY to i64*
  store i64* %lncZZ, i64** %Sp_Var
  %lnd00 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccVk_entry$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd01 = load i64*, i64** %Sp_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd00( i64* %Base_Arg, i64* %lnd01, i64* %Hp_Arg, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccVk_entry = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccVk_entry$def to i8*)
define internal ghccc void @ccVk_entry$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind
{
ccVk:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb71 = alloca i64, i32 1
  %lsb72 = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnd02 = load i64*, i64** %Hp_Var
  %lnd03 = getelementptr inbounds i64, i64* %lnd02, i32 6
  %lnd04 = ptrtoint i64* %lnd03 to i64
  %lnd05 = inttoptr i64 %lnd04 to i64*
  store i64* %lnd05, i64** %Hp_Var
  %lnd06 = load i64*, i64** %Sp_Var
  %lnd07 = getelementptr inbounds i64, i64* %lnd06, i32 1
  %lnd08 = bitcast i64* %lnd07 to i64*
  %lnd09 = load i64, i64* %lnd08, !tbaa !1
  store i64 %lnd09, i64* %lsb71
  %lnd0a = load i64*, i64** %Sp_Var
  %lnd0b = getelementptr inbounds i64, i64* %lnd0a, i32 0
  %lnd0c = bitcast i64* %lnd0b to i64*
  %lnd0d = load i64, i64* %lnd0c, !tbaa !1
  store i64 %lnd0d, i64* %lsb72
  %lnd0e = load i64*, i64** %Hp_Var
  %lnd0f = ptrtoint i64* %lnd0e to i64
  %lnd0g = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnd0h = bitcast i64* %lnd0g to i64*
  %lnd0i = load i64, i64* %lnd0h, !tbaa !4
  %lnd0j = icmp ugt i64 %lnd0f, %lnd0i
  br i1 %lnd0j, label %ccVO, label %ccVN
ccVN:
  %lnd0l = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb78_info$def to i64
  %lnd0k = load i64*, i64** %Hp_Var
  %lnd0m = getelementptr inbounds i64, i64* %lnd0k, i32 -5
  store i64 %lnd0l, i64* %lnd0m, !tbaa !2
  %lnd0o = load i64*, i64** %Sp_Var
  %lnd0p = getelementptr inbounds i64, i64* %lnd0o, i32 2
  %lnd0q = bitcast i64* %lnd0p to i64*
  %lnd0r = load i64, i64* %lnd0q, !tbaa !1
  %lnd0n = load i64*, i64** %Hp_Var
  %lnd0s = getelementptr inbounds i64, i64* %lnd0n, i32 -3
  store i64 %lnd0r, i64* %lnd0s, !tbaa !2
  %lnd0u = ptrtoint i8* @ghczmprim_GHCziTypes_ZC_con_info to i64
  %lnd0t = load i64*, i64** %Hp_Var
  %lnd0v = getelementptr inbounds i64, i64* %lnd0t, i32 -2
  store i64 %lnd0u, i64* %lnd0v, !tbaa !2
  %lnd0x = load i64, i64* %lsb71
  %lnd0w = load i64*, i64** %Hp_Var
  %lnd0y = getelementptr inbounds i64, i64* %lnd0w, i32 -1
  store i64 %lnd0x, i64* %lnd0y, !tbaa !2
  %lnd0A = load i64, i64* %lsb72
  %lnd0z = load i64*, i64** %Hp_Var
  %lnd0B = getelementptr inbounds i64, i64* %lnd0z, i32 0
  store i64 %lnd0A, i64* %lnd0B, !tbaa !2
  %lnd0C = load i64*, i64** %Hp_Var
  %lnd0D = getelementptr inbounds i64, i64* %lnd0C, i32 -5
  %lnd0E = ptrtoint i64* %lnd0D to i64
  store i64 %lnd0E, i64* %R3_Var
  %lnd0G = load i64*, i64** %Hp_Var
  %lnd0H = ptrtoint i64* %lnd0G to i64
  %lnd0I = add i64 %lnd0H, -14
  store i64 %lnd0I, i64* %R2_Var
  %lnd0J = load i64*, i64** %Sp_Var
  %lnd0K = getelementptr inbounds i64, i64* %lnd0J, i32 3
  %lnd0L = ptrtoint i64* %lnd0K to i64
  %lnd0M = inttoptr i64 %lnd0L to i64*
  store i64* %lnd0M, i64** %Sp_Var
  %lnd0N = bitcast i8* @base_GHCziBase_zpzp_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd0O = load i64*, i64** %Sp_Var
  %lnd0P = load i64*, i64** %Hp_Var
  %lnd0Q = load i64, i64* %R1_Var
  %lnd0R = load i64, i64* %R2_Var
  %lnd0S = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd0N( i64* %Base_Arg, i64* %lnd0O, i64* %lnd0P, i64 %lnd0Q, i64 %lnd0R, i64 %lnd0S, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccVO:
  %lnd0T = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 48, i64* %lnd0T, !tbaa !4
  %lnd0V = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccVj_info$def to i64
  %lnd0U = load i64*, i64** %Sp_Var
  %lnd0W = getelementptr inbounds i64, i64* %lnd0U, i32 1
  store i64 %lnd0V, i64* %lnd0W, !tbaa !1
  %lnd0X = load i64, i64* %lsb72
  store i64 %lnd0X, i64* %R2_Var
  %lnd0Y = load i64, i64* %lsb71
  store i64 %lnd0Y, i64* %R1_Var
  %lnd0Z = load i64*, i64** %Sp_Var
  %lnd10 = getelementptr inbounds i64, i64* %lnd0Z, i32 1
  %lnd11 = ptrtoint i64* %lnd10 to i64
  %lnd12 = inttoptr i64 %lnd11 to i64*
  store i64* %lnd12, i64** %Sp_Var
  %lnd13 = bitcast i8* @stg_gc_pp to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd14 = load i64*, i64** %Sp_Var
  %lnd15 = load i64*, i64** %Hp_Var
  %lnd16 = load i64, i64* %R1_Var
  %lnd17 = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd13( i64* %Base_Arg, i64* %lnd14, i64* %lnd15, i64 %lnd16, i64 %lnd17, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccVj_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccVj_info$def to i8*)
define internal ghccc void @ccVj_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccVj_info$def to i64)),i64 240), i64 1, i64 4294967328}>
{
ccVj:
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnd18 = load i64*, i64** %Sp_Var
  %lnd19 = getelementptr inbounds i64, i64* %lnd18, i32 0
  store i64 %R1_Arg, i64* %lnd19, !tbaa !1
  %lnd1a = load i64*, i64** %Sp_Var
  %lnd1b = getelementptr inbounds i64, i64* %lnd1a, i32 -1
  store i64 %R2_Arg, i64* %lnd1b, !tbaa !1
  %lnd1c = load i64*, i64** %Sp_Var
  %lnd1d = getelementptr inbounds i64, i64* %lnd1c, i32 -1
  %lnd1e = ptrtoint i64* %lnd1d to i64
  %lnd1f = inttoptr i64 %lnd1e to i64*
  store i64* %lnd1f, i64** %Sp_Var
  %lnd1g = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccVk_entry$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd1h = load i64*, i64** %Sp_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd1g( i64* %Base_Arg, i64* %lnd1h, i64* %Hp_Arg, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@sb7k_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb7k_info$def to i8*)
define internal ghccc void @sb7k_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb7k_info$def to i64)),i64 240), i64 4294967297, i64 12884901908}>
{
ccWm:
  %lsb7k = alloca i64, i32 1
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb6X = alloca i64, i32 1
  %lsb6Y = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnd1i = load i64, i64* %R1_Var
  store i64 %lnd1i, i64* %lsb7k
  %lnd1j = load i64*, i64** %Sp_Var
  %lnd1k = getelementptr inbounds i64, i64* %lnd1j, i32 -2
  %lnd1l = ptrtoint i64* %lnd1k to i64
  %lnd1m = icmp ult i64 %lnd1l, %SpLim_Arg
  br i1 %lnd1m, label %ccWn, label %ccWo
ccWo:
  %lnd1n = load i64*, i64** %Hp_Var
  %lnd1o = getelementptr inbounds i64, i64* %lnd1n, i32 4
  %lnd1p = ptrtoint i64* %lnd1o to i64
  %lnd1q = inttoptr i64 %lnd1p to i64*
  store i64* %lnd1q, i64** %Hp_Var
  %lnd1r = load i64*, i64** %Hp_Var
  %lnd1s = ptrtoint i64* %lnd1r to i64
  %lnd1t = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnd1u = bitcast i64* %lnd1t to i64*
  %lnd1v = load i64, i64* %lnd1u, !tbaa !4
  %lnd1w = icmp ugt i64 %lnd1s, %lnd1v
  br i1 %lnd1w, label %ccWq, label %ccWp
ccWp:
  %lnd1y = ptrtoint i8* @stg_upd_frame_info to i64
  %lnd1x = load i64*, i64** %Sp_Var
  %lnd1z = getelementptr inbounds i64, i64* %lnd1x, i32 -2
  store i64 %lnd1y, i64* %lnd1z, !tbaa !1
  %lnd1B = load i64, i64* %lsb7k
  %lnd1A = load i64*, i64** %Sp_Var
  %lnd1C = getelementptr inbounds i64, i64* %lnd1A, i32 -1
  store i64 %lnd1B, i64* %lnd1C, !tbaa !1
  %lnd1D = load i64, i64* %lsb7k
  %lnd1E = add i64 %lnd1D, 16
  %lnd1F = inttoptr i64 %lnd1E to i64*
  %lnd1G = load i64, i64* %lnd1F, !tbaa !5
  store i64 %lnd1G, i64* %lsb6X
  %lnd1H = load i64, i64* %lsb7k
  %lnd1I = add i64 %lnd1H, 24
  %lnd1J = inttoptr i64 %lnd1I to i64*
  %lnd1K = load i64, i64* %lnd1J, !tbaa !5
  store i64 %lnd1K, i64* %lsb6Y
  %lnd1M = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb7j_info$def to i64
  %lnd1L = load i64*, i64** %Hp_Var
  %lnd1N = getelementptr inbounds i64, i64* %lnd1L, i32 -3
  store i64 %lnd1M, i64* %lnd1N, !tbaa !2
  %lnd1P = load i64, i64* %lsb6X
  %lnd1O = load i64*, i64** %Hp_Var
  %lnd1Q = getelementptr inbounds i64, i64* %lnd1O, i32 -1
  store i64 %lnd1P, i64* %lnd1Q, !tbaa !2
  %lnd1S = load i64, i64* %lsb6Y
  %lnd1R = load i64*, i64** %Hp_Var
  %lnd1T = getelementptr inbounds i64, i64* %lnd1R, i32 0
  store i64 %lnd1S, i64* %lnd1T, !tbaa !2
  %lnd1U = load i64*, i64** %Hp_Var
  %lnd1V = getelementptr inbounds i64, i64* %lnd1U, i32 -3
  %lnd1W = ptrtoint i64* %lnd1V to i64
  store i64 %lnd1W, i64* %R3_Var
  %lnd1X = ptrtoint %ccWl_str_struct* @ccWl_str$def to i64
  store i64 %lnd1X, i64* %R2_Var
  %lnd1Y = load i64*, i64** %Sp_Var
  %lnd1Z = getelementptr inbounds i64, i64* %lnd1Y, i32 -2
  %lnd20 = ptrtoint i64* %lnd1Z to i64
  %lnd21 = inttoptr i64 %lnd20 to i64*
  store i64* %lnd21, i64** %Sp_Var
  %lnd22 = bitcast i8* @ghczmprim_GHCziCString_unpackAppendCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd23 = load i64*, i64** %Sp_Var
  %lnd24 = load i64*, i64** %Hp_Var
  %lnd25 = load i64, i64* %R1_Var
  %lnd26 = load i64, i64* %R2_Var
  %lnd27 = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd22( i64* %Base_Arg, i64* %lnd23, i64* %lnd24, i64 %lnd25, i64 %lnd26, i64 %lnd27, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccWq:
  %lnd28 = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 32, i64* %lnd28, !tbaa !4
  br label %ccWn
ccWn:
  %lnd29 = load i64, i64* %lsb7k
  store i64 %lnd29, i64* %R1_Var
  %lnd2a = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lnd2b = bitcast i64* %lnd2a to i64*
  %lnd2c = load i64, i64* %lnd2b, !tbaa !4
  %lnd2d = inttoptr i64 %lnd2c to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd2e = load i64*, i64** %Sp_Var
  %lnd2f = load i64*, i64** %Hp_Var
  %lnd2g = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd2d( i64* %Base_Arg, i64* %lnd2e, i64* %lnd2f, i64 %lnd2g, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@Disk_readzudisk3_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_readzudisk3_info$def to i8*)
define ghccc void @Disk_readzudisk3_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_readzudisk3_info$def to i64)),i64 80), i64 8589934606, i64 0, i64 67553998705524751}>
{
ccWr:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnd2h = load i64*, i64** %Hp_Var
  %lnd2i = getelementptr inbounds i64, i64* %lnd2h, i32 4
  %lnd2j = ptrtoint i64* %lnd2i to i64
  %lnd2k = inttoptr i64 %lnd2j to i64*
  store i64* %lnd2k, i64** %Hp_Var
  %lnd2l = load i64*, i64** %Hp_Var
  %lnd2m = ptrtoint i64* %lnd2l to i64
  %lnd2n = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnd2o = bitcast i64* %lnd2n to i64*
  %lnd2p = load i64, i64* %lnd2o, !tbaa !4
  %lnd2q = icmp ugt i64 %lnd2m, %lnd2p
  br i1 %lnd2q, label %ccWv, label %ccWu
ccWu:
  %lnd2s = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb7k_info$def to i64
  %lnd2r = load i64*, i64** %Hp_Var
  %lnd2t = getelementptr inbounds i64, i64* %lnd2r, i32 -3
  store i64 %lnd2s, i64* %lnd2t, !tbaa !2
  %lnd2v = load i64, i64* %R2_Var
  %lnd2u = load i64*, i64** %Hp_Var
  %lnd2w = getelementptr inbounds i64, i64* %lnd2u, i32 -1
  store i64 %lnd2v, i64* %lnd2w, !tbaa !2
  %lnd2y = load i64, i64* %R3_Var
  %lnd2x = load i64*, i64** %Hp_Var
  %lnd2z = getelementptr inbounds i64, i64* %lnd2x, i32 0
  store i64 %lnd2y, i64* %lnd2z, !tbaa !2
  %lnd2A = load i64*, i64** %Hp_Var
  %lnd2B = getelementptr inbounds i64, i64* %lnd2A, i32 -3
  %lnd2C = ptrtoint i64* %lnd2B to i64
  store i64 %lnd2C, i64* %R3_Var
  %lnd2D = ptrtoint %rb0E_closure_struct* @rb0E_closure$def to i64
  %lnd2E = add i64 %lnd2D, 2
  store i64 %lnd2E, i64* %R2_Var
  %lnd2F = bitcast i8* @base_GHCziErr_error_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd2G = load i64*, i64** %Hp_Var
  %lnd2H = load i64, i64* %R1_Var
  %lnd2I = load i64, i64* %R2_Var
  %lnd2J = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd2F( i64* %Base_Arg, i64* %Sp_Arg, i64* %lnd2G, i64 %lnd2H, i64 %lnd2I, i64 %lnd2J, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccWv:
  %lnd2K = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 32, i64* %lnd2K, !tbaa !4
  %lnd2L = load i64, i64* %R3_Var
  store i64 %lnd2L, i64* %R3_Var
  %lnd2M = load i64, i64* %R2_Var
  store i64 %lnd2M, i64* %R2_Var
  %lnd2N = ptrtoint %Disk_readzudisk3_closure_struct* @Disk_readzudisk3_closure$def to i64
  store i64 %lnd2N, i64* %R1_Var
  %lnd2O = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lnd2P = bitcast i64* %lnd2O to i64*
  %lnd2Q = load i64, i64* %lnd2P, !tbaa !4
  %lnd2R = inttoptr i64 %lnd2Q to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd2S = load i64*, i64** %Hp_Var
  %lnd2T = load i64, i64* %R1_Var
  %lnd2U = load i64, i64* %R2_Var
  %lnd2V = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd2R( i64* %Base_Arg, i64* %Sp_Arg, i64* %lnd2S, i64 %lnd2T, i64 %lnd2U, i64 %lnd2V, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_readzudisk2_closure_struct = type <{i64, i64, i64}>
@Disk_readzudisk2_closure$def = internal global %Disk_readzudisk2_closure_struct<{i64 ptrtoint (i8* @Word_W_static_info to i64), i64 ptrtoint (i8* @Word_bs2i2_closure to i64), i64 0}>
@Disk_readzudisk2_closure = alias i8* bitcast (%Disk_readzudisk2_closure_struct* @Disk_readzudisk2_closure$def to i8*)
%Disk_zdwreadzudisk_closure_struct = type <{i64, i64}>
@Disk_zdwreadzudisk_closure$def = internal global %Disk_zdwreadzudisk_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwreadzudisk_info$def to i64), i64 0}>
@Disk_zdwreadzudisk_closure = alias i8* bitcast (%Disk_zdwreadzudisk_closure_struct* @Disk_zdwreadzudisk_closure$def to i8*)
@Disk_zdwreadzudisk_slow = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwreadzudisk_slow$def to i8*)
define ghccc void @Disk_zdwreadzudisk_slow$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind
{
cd43:
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnd6G = load i64*, i64** %Sp_Var
  %lnd6H = getelementptr inbounds i64, i64* %lnd6G, i32 3
  %lnd6I = bitcast i64* %lnd6H to i64*
  %lnd6J = load i64, i64* %lnd6I, !tbaa !1
  store i64 %lnd6J, i64* %R5_Var
  %lnd6K = load i64*, i64** %Sp_Var
  %lnd6L = getelementptr inbounds i64, i64* %lnd6K, i32 2
  %lnd6M = bitcast i64* %lnd6L to i64*
  %lnd6N = load i64, i64* %lnd6M, !tbaa !1
  store i64 %lnd6N, i64* %R4_Var
  %lnd6O = load i64*, i64** %Sp_Var
  %lnd6P = getelementptr inbounds i64, i64* %lnd6O, i32 1
  %lnd6Q = bitcast i64* %lnd6P to i64*
  %lnd6R = load i64, i64* %lnd6Q, !tbaa !1
  store i64 %lnd6R, i64* %R3_Var
  %lnd6S = load i64*, i64** %Sp_Var
  %lnd6T = getelementptr inbounds i64, i64* %lnd6S, i32 0
  %lnd6U = bitcast i64* %lnd6T to i64*
  %lnd6V = load i64, i64* %lnd6U, !tbaa !1
  store i64 %lnd6V, i64* %R2_Var
  %lnd6W = load i64, i64* %R1_Var
  store i64 %lnd6W, i64* %R1_Var
  %lnd6X = load i64*, i64** %Sp_Var
  %lnd6Y = getelementptr inbounds i64, i64* %lnd6X, i32 4
  %lnd6Z = ptrtoint i64* %lnd6Y to i64
  %lnd70 = inttoptr i64 %lnd6Z to i64*
  store i64* %lnd70, i64** %Sp_Var
  %lnd71 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwreadzudisk_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd72 = load i64*, i64** %Sp_Var
  %lnd73 = load i64, i64* %R1_Var
  %lnd74 = load i64, i64* %R2_Var
  %lnd75 = load i64, i64* %R3_Var
  %lnd76 = load i64, i64* %R4_Var
  %lnd77 = load i64, i64* %R5_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd71( i64* %Base_Arg, i64* %lnd72, i64* %Hp_Arg, i64 %lnd73, i64 %lnd74, i64 %lnd75, i64 %lnd76, i64 %lnd77, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@sb7W_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb7W_info$def to i8*)
define internal ghccc void @sb7W_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 4294967299, i64 8589934592, i64 14}>
{
cd4O:
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lcd4R = alloca i64, i32 1
  %lcd4S = alloca i64, i32 1
  %lud55 = alloca i64, i32 1
  %lud56 = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %lud52 = alloca i64, i32 1
  %lsb7U = alloca i64, i32 1
  %lud53 = alloca i64, i32 1
  %Base_Var = alloca i64*, i32 1
  store i64* %Base_Arg, i64** %Base_Var
  %lud54 = alloca i64, i32 1
  %SpLim_Var = alloca i64, i32 1
  store i64 %SpLim_Arg, i64* %SpLim_Var
  %lud57 = alloca i64, i32 1
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lud58 = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnd78 = load i64*, i64** %Sp_Var
  %lnd79 = getelementptr inbounds i64, i64* %lnd78, i32 -1
  %lnd7a = ptrtoint i64* %lnd79 to i64
  %lnd7b = load i64, i64* %SpLim_Var
  %lnd7c = icmp ult i64 %lnd7a, %lnd7b
  br i1 %lnd7c, label %cd4X, label %cd4Y
cd4Y:
  %lnd7e = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd4U_info$def to i64
  %lnd7d = load i64*, i64** %Sp_Var
  %lnd7f = getelementptr inbounds i64, i64* %lnd7d, i32 -1
  store i64 %lnd7e, i64* %lnd7f, !tbaa !1
  %lnd7g = load i64*, i64** %Sp_Var
  %lnd7h = getelementptr inbounds i64, i64* %lnd7g, i32 -1
  %lnd7i = ptrtoint i64* %lnd7h to i64
  %lnd7j = inttoptr i64 %lnd7i to i64*
  store i64* %lnd7j, i64** %Sp_Var
  %lnd7m = load i64, i64* %R1_Var
  %lnd7n = add i64 %lnd7m, 7
  %lnd7o = inttoptr i64 %lnd7n to i64*
  %lnd7p = load i64, i64* %lnd7o, !tbaa !3
  %lnd7q = trunc i64 %lnd7p to i32
  %lnd7r = sext i32 %lnd7q to i64
  store i64 %lnd7r, i64* %lcd4R
  %lnd7u = load i64, i64* %R1_Var
  %lnd7v = add i64 %lnd7u, 15
  %lnd7w = inttoptr i64 %lnd7v to i64*
  %lnd7x = load i64, i64* %lnd7w, !tbaa !3
  store i64 %lnd7x, i64* %lcd4S
  %lnd7y = load i64*, i64** %Base_Var
  %lnd7z = getelementptr inbounds i64, i64* %lnd7y, i32 109
  %lnd7A = bitcast i64* %lnd7z to i64*
  %lnd7B = load i64, i64* %lnd7A, !tbaa !4
  store i64 %lnd7B, i64* %lud55
  %lnd7C = load i64, i64* %lud55
  %lnd7D = add i64 %lnd7C, 24
  %lnd7E = inttoptr i64 %lnd7D to i64*
  %lnd7F = load i64, i64* %lnd7E, !tbaa !5
  %lnd7G = add i64 %lnd7F, 16
  %lnd7H = load i64*, i64** %Sp_Var
  %lnd7I = ptrtoint i64* %lnd7H to i64
  %lnd7J = inttoptr i64 %lnd7G to i64*
  store i64 %lnd7I, i64* %lnd7J, !tbaa !5
  %lnd7K = load i64*, i64** %Base_Var
  %lnd7L = getelementptr inbounds i64, i64* %lnd7K, i32 111
  %lnd7M = bitcast i64* %lnd7L to i64*
  %lnd7N = load i64, i64* %lnd7M, !tbaa !4
  store i64 %lnd7N, i64* %lud56
  %lnd7O = load i64, i64* %lud56
  %lnd7P = add i64 %lnd7O, 8
  %lnd7Q = load i64*, i64** %Hp_Var
  %lnd7R = getelementptr inbounds i64, i64* %lnd7Q, i32 1
  %lnd7S = ptrtoint i64* %lnd7R to i64
  %lnd7T = inttoptr i64 %lnd7P to i64*
  store i64 %lnd7S, i64* %lnd7T, !tbaa !5
  %lnd7U = load i64, i64* %lud55
  %lnd7V = add i64 %lnd7U, 104
  %lnd7W = load i64, i64* %lud55
  %lnd7X = add i64 %lnd7W, 104
  %lnd7Y = inttoptr i64 %lnd7X to i64*
  %lnd7Z = load i64, i64* %lnd7Y, !tbaa !5
  %lnd80 = load i64*, i64** %Hp_Var
  %lnd81 = getelementptr inbounds i64, i64* %lnd80, i32 1
  %lnd82 = ptrtoint i64* %lnd81 to i64
  %lnd83 = load i64, i64* %lud56
  %lnd84 = inttoptr i64 %lnd83 to i64*
  %lnd85 = load i64, i64* %lnd84, !tbaa !5
  %lnd86 = sub i64 %lnd82, %lnd85
  %lnd87 = sub i64 %lnd7Z, %lnd86
  %lnd88 = inttoptr i64 %lnd7V to i64*
  store i64 %lnd87, i64* %lnd88, !tbaa !5
  %lnd89 = load i64*, i64** %Base_Var
  %lnd8a = ptrtoint i64* %lnd89 to i64
  %lnd8b = inttoptr i64 %lnd8a to i8*
  %lnd8c = bitcast i8* @suspendThread to i8* (i8*, i64)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lnd8d = call ccc i8* (i8*, i64) %lnd8c( i8* %lnd8b, i64 0 ) nounwind
  %lnd8e = ptrtoint i8* %lnd8d to i64
  store i64 %lnd8e, i64* %lud52
  %lnd8f = load i64, i64* %lcd4R
  %lnd8g = load i64, i64* %lcd4S
  %lnd8h = inttoptr i64 %lnd8g to i8*
  %lnd8i = bitcast i8* @read to i64 (i64, i8*, i64)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lnd8j = call ccc i64 (i64, i8*, i64) %lnd8i( i64 %lnd8f, i8* %lnd8h, i64 4096 ) nounwind
  store i64 %lnd8j, i64* %lsb7U
  %lnd8k = load i64, i64* %lud52
  %lnd8l = inttoptr i64 %lnd8k to i8*
  %lnd8m = bitcast i8* @resumeThread to i8* (i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lnd8n = call ccc i8* (i8*) %lnd8m( i8* %lnd8l ) nounwind
  %lnd8o = ptrtoint i8* %lnd8n to i64
  store i64 %lnd8o, i64* %lud53
  %lnd8p = load i64, i64* %lud53
  %lnd8q = inttoptr i64 %lnd8p to i64*
  store i64* %lnd8q, i64** %Base_Var
  %lnd8r = load i64*, i64** %Base_Var
  %lnd8s = getelementptr inbounds i64, i64* %lnd8r, i32 109
  %lnd8t = bitcast i64* %lnd8s to i64*
  %lnd8u = load i64, i64* %lnd8t, !tbaa !4
  store i64 %lnd8u, i64* %lud55
  %lnd8v = load i64, i64* %lud55
  %lnd8w = add i64 %lnd8v, 24
  %lnd8x = inttoptr i64 %lnd8w to i64*
  %lnd8y = load i64, i64* %lnd8x, !tbaa !5
  store i64 %lnd8y, i64* %lud54
  %lnd8z = load i64, i64* %lud54
  %lnd8A = add i64 %lnd8z, 16
  %lnd8B = inttoptr i64 %lnd8A to i64*
  %lnd8C = load i64, i64* %lnd8B, !tbaa !5
  %lnd8D = inttoptr i64 %lnd8C to i64*
  store i64* %lnd8D, i64** %Sp_Var
  %lnd8E = load i64, i64* %lud54
  %lnd8F = add i64 %lnd8E, 192
  store i64 %lnd8F, i64* %SpLim_Var
  %lnd8G = load i64*, i64** %Base_Var
  %lnd8H = getelementptr inbounds i64, i64* %lnd8G, i32 113
  store i64 0, i64* %lnd8H, !tbaa !4
  %lnd8I = load i64*, i64** %Base_Var
  %lnd8J = getelementptr inbounds i64, i64* %lnd8I, i32 111
  %lnd8K = bitcast i64* %lnd8J to i64*
  %lnd8L = load i64, i64* %lnd8K, !tbaa !4
  store i64 %lnd8L, i64* %lud56
  %lnd8M = load i64, i64* %lud56
  %lnd8N = add i64 %lnd8M, 8
  %lnd8O = inttoptr i64 %lnd8N to i64*
  %lnd8P = load i64, i64* %lnd8O, !tbaa !5
  store i64 %lnd8P, i64* %lud57
  %lnd8Q = load i64, i64* %lud57
  %lnd8R = add i64 %lnd8Q, -8
  %lnd8S = inttoptr i64 %lnd8R to i64*
  store i64* %lnd8S, i64** %Hp_Var
  %lnd8T = load i64, i64* %lud56
  %lnd8U = inttoptr i64 %lnd8T to i64*
  %lnd8V = load i64, i64* %lnd8U, !tbaa !5
  store i64 %lnd8V, i64* %lud58
  %lnd8X = load i64, i64* %lud58
  %lnd8Y = load i64, i64* %lud56
  %lnd8Z = add i64 %lnd8Y, 48
  %lnd90 = inttoptr i64 %lnd8Z to i32*
  %lnd91 = load i32, i32* %lnd90, !tbaa !5
  %lnd92 = sext i32 %lnd91 to i64
  %lnd93 = shl i64 %lnd92, 12
  %lnd94 = add i64 %lnd93, -1
  %lnd95 = add i64 %lnd8X, %lnd94
  %lnd8W = load i64*, i64** %Base_Var
  %lnd96 = getelementptr inbounds i64, i64* %lnd8W, i32 107
  store i64 %lnd95, i64* %lnd96, !tbaa !4
  %lnd97 = load i64, i64* %lud55
  %lnd98 = add i64 %lnd97, 104
  %lnd99 = load i64, i64* %lud55
  %lnd9a = add i64 %lnd99, 104
  %lnd9b = inttoptr i64 %lnd9a to i64*
  %lnd9c = load i64, i64* %lnd9b, !tbaa !5
  %lnd9d = load i64, i64* %lud57
  %lnd9e = load i64, i64* %lud58
  %lnd9f = sub i64 %lnd9d, %lnd9e
  %lnd9g = add i64 %lnd9c, %lnd9f
  %lnd9h = inttoptr i64 %lnd98 to i64*
  store i64 %lnd9g, i64* %lnd9h, !tbaa !5
  %lnd9i = load i64, i64* %lsb7U
  store i64 %lnd9i, i64* %R1_Var
  %lnd9j = load i64*, i64** %Sp_Var
  %lnd9k = getelementptr inbounds i64, i64* %lnd9j, i32 0
  %lnd9l = bitcast i64* %lnd9k to i64*
  %lnd9m = load i64, i64* %lnd9l, !tbaa !1
  %lnd9n = inttoptr i64 %lnd9m to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd9o = load i64*, i64** %Base_Var
  %lnd9p = load i64*, i64** %Sp_Var
  %lnd9q = load i64*, i64** %Hp_Var
  %lnd9r = load i64, i64* %R1_Var
  %lnd9s = load i64, i64* %SpLim_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd9n( i64* %lnd9o, i64* %lnd9p, i64* %lnd9q, i64 %lnd9r, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %lnd9s ) nounwind
  ret void
cd4X:
  %lnd9t = load i64, i64* %R1_Var
  store i64 %lnd9t, i64* %R1_Var
  %lnd9u = load i64*, i64** %Base_Var
  %lnd9v = getelementptr inbounds i64, i64* %lnd9u, i32 -1
  %lnd9w = bitcast i64* %lnd9v to i64*
  %lnd9x = load i64, i64* %lnd9w, !tbaa !4
  %lnd9y = inttoptr i64 %lnd9x to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd9z = load i64*, i64** %Base_Var
  %lnd9A = load i64*, i64** %Sp_Var
  %lnd9B = load i64*, i64** %Hp_Var
  %lnd9C = load i64, i64* %R1_Var
  %lnd9D = load i64, i64* %SpLim_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd9y( i64* %lnd9z, i64* %lnd9A, i64* %lnd9B, i64 %lnd9C, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %lnd9D ) nounwind
  ret void
}
@cd4U_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd4U_info$def to i8*)
define internal ghccc void @cd4U_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
cd4U:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnd9E = load i64*, i64** %Hp_Var
  %lnd9F = getelementptr inbounds i64, i64* %lnd9E, i32 2
  %lnd9G = ptrtoint i64* %lnd9F to i64
  %lnd9H = inttoptr i64 %lnd9G to i64*
  store i64* %lnd9H, i64** %Hp_Var
  %lnd9I = load i64*, i64** %Hp_Var
  %lnd9J = ptrtoint i64* %lnd9I to i64
  %lnd9K = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnd9L = bitcast i64* %lnd9K to i64*
  %lnd9M = load i64, i64* %lnd9L, !tbaa !4
  %lnd9N = icmp ugt i64 %lnd9J, %lnd9M
  br i1 %lnd9N, label %cd51, label %cd50
cd50:
  %lnd9P = ptrtoint i8* @base_GHCziInt_I64zh_con_info to i64
  %lnd9O = load i64*, i64** %Hp_Var
  %lnd9Q = getelementptr inbounds i64, i64* %lnd9O, i32 -1
  store i64 %lnd9P, i64* %lnd9Q, !tbaa !2
  %lnd9S = load i64, i64* %R1_Var
  %lnd9R = load i64*, i64** %Hp_Var
  %lnd9T = getelementptr inbounds i64, i64* %lnd9R, i32 0
  store i64 %lnd9S, i64* %lnd9T, !tbaa !2
  %lnd9V = load i64*, i64** %Hp_Var
  %lnd9W = ptrtoint i64* %lnd9V to i64
  %lnd9X = add i64 %lnd9W, -7
  store i64 %lnd9X, i64* %R1_Var
  %lnd9Y = load i64*, i64** %Sp_Var
  %lnd9Z = getelementptr inbounds i64, i64* %lnd9Y, i32 1
  %lnda0 = ptrtoint i64* %lnd9Z to i64
  %lnda1 = inttoptr i64 %lnda0 to i64*
  store i64* %lnda1, i64** %Sp_Var
  %lnda2 = load i64*, i64** %Sp_Var
  %lnda3 = getelementptr inbounds i64, i64* %lnda2, i32 0
  %lnda4 = bitcast i64* %lnda3 to i64*
  %lnda5 = load i64, i64* %lnda4, !tbaa !1
  %lnda6 = inttoptr i64 %lnda5 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnda7 = load i64*, i64** %Sp_Var
  %lnda8 = load i64*, i64** %Hp_Var
  %lnda9 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnda6( i64* %Base_Arg, i64* %lnda7, i64* %lnda8, i64 %lnda9, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cd51:
  %lndaa = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 16, i64* %lndaa, !tbaa !4
  %lndab = load i64, i64* %R1_Var
  store i64 %lndab, i64* %R1_Var
  %lndac = bitcast i8* @stg_gc_unbx_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndad = load i64*, i64** %Sp_Var
  %lndae = load i64*, i64** %Hp_Var
  %lndaf = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndac( i64* %Base_Arg, i64* %lndad, i64* %lndae, i64 %lndaf, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@sb8n_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb8n_info$def to i8*)
define internal ghccc void @sb8n_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 1, i64 17}>
{
cd5V:
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lndag = load i64*, i64** %Sp_Var
  %lndah = getelementptr inbounds i64, i64* %lndag, i32 -2
  %lndai = ptrtoint i64* %lndah to i64
  %lndaj = icmp ult i64 %lndai, %SpLim_Arg
  br i1 %lndaj, label %cd5W, label %cd5X
cd5X:
  %lndal = ptrtoint i8* @stg_upd_frame_info to i64
  %lndak = load i64*, i64** %Sp_Var
  %lndam = getelementptr inbounds i64, i64* %lndak, i32 -2
  store i64 %lndal, i64* %lndam, !tbaa !1
  %lndao = load i64, i64* %R1_Var
  %lndan = load i64*, i64** %Sp_Var
  %lndap = getelementptr inbounds i64, i64* %lndan, i32 -1
  store i64 %lndao, i64* %lndap, !tbaa !1
  %lndas = load i64, i64* %R1_Var
  %lndat = add i64 %lndas, 16
  %lndau = inttoptr i64 %lndat to i64*
  %lndav = load i64, i64* %lndau, !tbaa !3
  store i64 %lndav, i64* %R2_Var
  %lndaw = load i64*, i64** %Sp_Var
  %lndax = getelementptr inbounds i64, i64* %lndaw, i32 -2
  %lnday = ptrtoint i64* %lndax to i64
  %lndaz = inttoptr i64 %lnday to i64*
  store i64* %lndaz, i64** %Sp_Var
  %lndaA = bitcast i8* @integerzmgmp_GHCziIntegerziType_zdwbigNatToInteger_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndaB = load i64*, i64** %Sp_Var
  %lndaC = load i64, i64* %R1_Var
  %lndaD = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndaA( i64* %Base_Arg, i64* %lndaB, i64* %Hp_Arg, i64 %lndaC, i64 %lndaD, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cd5W:
  %lndaE = load i64, i64* %R1_Var
  store i64 %lndaE, i64* %R1_Var
  %lndaF = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lndaG = bitcast i64* %lndaF to i64*
  %lndaH = load i64, i64* %lndaG, !tbaa !4
  %lndaI = inttoptr i64 %lndaH to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndaJ = load i64*, i64** %Sp_Var
  %lndaK = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndaI( i64* %Base_Arg, i64* %lndaJ, i64* %Hp_Arg, i64 %lndaK, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@Disk_zdwreadzudisk_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwreadzudisk_info$def to i8*)
define ghccc void @Disk_zdwreadzudisk_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwreadzudisk_slow$def to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwreadzudisk_info$def to i64)),i64 0), i64 68, i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwreadzudisk_info$def to i64)),i64 176), i64 21474836480, i64 0, i64 1112710062276623}>
{
cd5Y:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lndaL = load i64*, i64** %Sp_Var
  %lndaM = getelementptr inbounds i64, i64* %lndaL, i32 -4
  %lndaN = ptrtoint i64* %lndaM to i64
  %lndaO = icmp ult i64 %lndaN, %SpLim_Arg
  br i1 %lndaO, label %cd5Z, label %cd60
cd60:
  %lndaQ = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd47_info$def to i64
  %lndaP = load i64*, i64** %Sp_Var
  %lndaR = getelementptr inbounds i64, i64* %lndaP, i32 -4
  store i64 %lndaQ, i64* %lndaR, !tbaa !1
  %lndaS = add i64 %R3_Arg, 8
  %lndaT = inttoptr i64 %lndaS to i64*
  %lndaU = load i64, i64* %lndaT, !tbaa !3
  store i64 %lndaU, i64* %R1_Var
  %lndaV = load i64*, i64** %Sp_Var
  %lndaW = getelementptr inbounds i64, i64* %lndaV, i32 -3
  store i64 %R2_Arg, i64* %lndaW, !tbaa !1
  %lndaX = load i64*, i64** %Sp_Var
  %lndaY = getelementptr inbounds i64, i64* %lndaX, i32 -2
  store i64 %R3_Arg, i64* %lndaY, !tbaa !1
  %lndaZ = load i64*, i64** %Sp_Var
  %lndb0 = getelementptr inbounds i64, i64* %lndaZ, i32 -1
  store i64 %R5_Arg, i64* %lndb0, !tbaa !1
  %lndb1 = load i64*, i64** %Sp_Var
  %lndb2 = getelementptr inbounds i64, i64* %lndb1, i32 -4
  %lndb3 = ptrtoint i64* %lndb2 to i64
  %lndb4 = inttoptr i64 %lndb3 to i64*
  store i64* %lndb4, i64** %Sp_Var
  %lndb5 = load i64, i64* %R1_Var
  %lndb6 = and i64 %lndb5, 7
  %lndb7 = icmp ne i64 %lndb6, 0
  br i1 %lndb7, label %ud6E, label %cd48
cd48:
  %lndb9 = load i64, i64* %R1_Var
  %lndba = inttoptr i64 %lndb9 to i64*
  %lndbb = load i64, i64* %lndba, !tbaa !3
  %lndbc = inttoptr i64 %lndbb to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndbd = load i64*, i64** %Sp_Var
  %lndbe = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndbc( i64* %Base_Arg, i64* %lndbd, i64* %Hp_Arg, i64 %lndbe, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ud6E:
  %lndbf = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd47_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndbg = load i64*, i64** %Sp_Var
  %lndbh = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndbf( i64* %Base_Arg, i64* %lndbg, i64* %Hp_Arg, i64 %lndbh, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cd5Z:
  %lndbi = ptrtoint %Disk_zdwreadzudisk_closure_struct* @Disk_zdwreadzudisk_closure$def to i64
  store i64 %lndbi, i64* %R1_Var
  %lndbj = load i64*, i64** %Sp_Var
  %lndbk = getelementptr inbounds i64, i64* %lndbj, i32 -4
  store i64 %R2_Arg, i64* %lndbk, !tbaa !1
  %lndbl = load i64*, i64** %Sp_Var
  %lndbm = getelementptr inbounds i64, i64* %lndbl, i32 -3
  store i64 %R3_Arg, i64* %lndbm, !tbaa !1
  %lndbn = load i64*, i64** %Sp_Var
  %lndbo = getelementptr inbounds i64, i64* %lndbn, i32 -2
  store i64 %R4_Arg, i64* %lndbo, !tbaa !1
  %lndbp = load i64*, i64** %Sp_Var
  %lndbq = getelementptr inbounds i64, i64* %lndbp, i32 -1
  store i64 %R5_Arg, i64* %lndbq, !tbaa !1
  %lndbr = load i64*, i64** %Sp_Var
  %lndbs = getelementptr inbounds i64, i64* %lndbr, i32 -4
  %lndbt = ptrtoint i64* %lndbs to i64
  %lndbu = inttoptr i64 %lndbt to i64*
  store i64* %lndbu, i64** %Sp_Var
  %lndbv = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lndbw = bitcast i64* %lndbv to i64*
  %lndbx = load i64, i64* %lndbw, !tbaa !4
  %lndby = inttoptr i64 %lndbx to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndbz = load i64*, i64** %Sp_Var
  %lndbA = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndby( i64* %Base_Arg, i64* %lndbz, i64* %Hp_Arg, i64 %lndbA, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cd47_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd47_info$def to i8*)
define internal ghccc void @cd47_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd47_info$def to i64)),i64 176), i64 67, i64 1077525690187808}>
{
cd47:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb7v = alloca i64, i32 1
  %lsb7w = alloca i64, i32 1
  %lsb7x = alloca i64, i32 1
  %lsb7m = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lndbB = load i64*, i64** %Hp_Var
  %lndbC = getelementptr inbounds i64, i64* %lndbB, i32 4
  %lndbD = ptrtoint i64* %lndbC to i64
  %lndbE = inttoptr i64 %lndbD to i64*
  store i64* %lndbE, i64** %Hp_Var
  %lndbF = load i64*, i64** %Hp_Var
  %lndbG = ptrtoint i64* %lndbF to i64
  %lndbH = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lndbI = bitcast i64* %lndbH to i64*
  %lndbJ = load i64, i64* %lndbI, !tbaa !4
  %lndbK = icmp ugt i64 %lndbG, %lndbJ
  br i1 %lndbK, label %cd63, label %cd62
cd62:
  %lndbN = load i64, i64* %R1_Var
  %lndbO = add i64 %lndbN, 15
  %lndbP = inttoptr i64 %lndbO to i64*
  %lndbQ = load i64, i64* %lndbP, !tbaa !3
  store i64 %lndbQ, i64* %lsb7v
  %lndbT = load i64, i64* %R1_Var
  %lndbU = add i64 %lndbT, 23
  %lndbV = inttoptr i64 %lndbU to i64*
  %lndbW = load i64, i64* %lndbV, !tbaa !3
  store i64 %lndbW, i64* %lsb7w
  %lndbZ = load i64, i64* %R1_Var
  %lndc0 = add i64 %lndbZ, 7
  %lndc1 = inttoptr i64 %lndc0 to i64*
  %lndc2 = load i64, i64* %lndc1, !tbaa !3
  %lndc3 = add i64 %lndc2, 1
  store i64 %lndc3, i64* %lsb7x
  %lndc5 = ptrtoint i8* @Disk_Stats_con_info to i64
  %lndc4 = load i64*, i64** %Hp_Var
  %lndc6 = getelementptr inbounds i64, i64* %lndc4, i32 -3
  store i64 %lndc5, i64* %lndc6, !tbaa !2
  %lndc8 = load i64, i64* %lsb7x
  %lndc7 = load i64*, i64** %Hp_Var
  %lndc9 = getelementptr inbounds i64, i64* %lndc7, i32 -2
  store i64 %lndc8, i64* %lndc9, !tbaa !2
  %lndcb = load i64, i64* %lsb7v
  %lndca = load i64*, i64** %Hp_Var
  %lndcc = getelementptr inbounds i64, i64* %lndca, i32 -1
  store i64 %lndcb, i64* %lndcc, !tbaa !2
  %lndce = load i64, i64* %lsb7w
  %lndcd = load i64*, i64** %Hp_Var
  %lndcf = getelementptr inbounds i64, i64* %lndcd, i32 0
  store i64 %lndce, i64* %lndcf, !tbaa !2
  %lndcg = getelementptr inbounds i64, i64* %Sp_Arg, i32 2
  %lndch = bitcast i64* %lndcg to i64*
  %lndci = load i64, i64* %lndch, !tbaa !1
  store i64 %lndci, i64* %lsb7m
  %lndcj = load i64, i64* %lsb7m
  %lndck = add i64 %lndcj, 8
  %lndcm = load i64*, i64** %Hp_Var
  %lndcn = ptrtoint i64* %lndcm to i64
  %lndco = add i64 %lndcn, -23
  %lndcp = inttoptr i64 %lndck to i64*
  store i64 %lndco, i64* %lndcp, !tbaa !5
  %lndcq = ptrtoint i64* %Base_Arg to i64
  %lndcr = inttoptr i64 %lndcq to i8*
  %lndcs = load i64, i64* %lsb7m
  %lndct = inttoptr i64 %lndcs to i8*
  %lndcu = bitcast i8* @dirty_MUT_VAR to void (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  call ccc void (i8*, i8*) %lndcu( i8* %lndcr, i8* %lndct ) nounwind
  %lndcv = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd4l_info$def to i64
  %lndcw = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lndcv, i64* %lndcw, !tbaa !1
  store i64 4096, i64* %R1_Var
  %lndcx = bitcast i8* @stg_newPinnedByteArrayzh to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndcy = load i64*, i64** %Hp_Var
  %lndcz = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndcx( i64* %Base_Arg, i64* %Sp_Arg, i64* %lndcy, i64 %lndcz, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cd63:
  %lndcA = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 32, i64* %lndcA, !tbaa !4
  %lndcB = load i64, i64* %R1_Var
  store i64 %lndcB, i64* %R1_Var
  %lndcC = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndcD = load i64*, i64** %Hp_Var
  %lndcE = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndcC( i64* %Base_Arg, i64* %Sp_Arg, i64* %lndcD, i64 %lndcE, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cd4l_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd4l_info$def to i8*)
define internal ghccc void @cd4l_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd4l_info$def to i64)),i64 176), i64 195, i64 1077525690187808}>
{
cd4l:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %lndcF = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd4n_info$def to i64
  %lndcG = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lndcF, i64* %lndcG, !tbaa !1
  %lndcH = getelementptr inbounds i64, i64* %Sp_Arg, i32 3
  %lndcI = bitcast i64* %lndcH to i64*
  %lndcJ = load i64, i64* %lndcI, !tbaa !1
  store i64 %lndcJ, i64* %R3_Var
  %lndcK = ptrtoint %Disk_readzudisk4_closure_struct* @Disk_readzudisk4_closure$def to i64
  %lndcL = add i64 %lndcK, 1
  store i64 %lndcL, i64* %R2_Var
  %lndcM = getelementptr inbounds i64, i64* %Sp_Arg, i32 2
  store i64 %R1_Arg, i64* %lndcM, !tbaa !1
  %lndcN = bitcast i8* @integerzmgmp_GHCziIntegerziType_timesInteger_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndcO = load i64, i64* %R2_Var
  %lndcP = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndcN( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lndcO, i64 %lndcP, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cd4n_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd4n_info$def to i8*)
define internal ghccc void @cd4n_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd4n_info$def to i64)),i64 176), i64 67, i64 1059933504143392}>
{
cd4n:
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %lndcQ = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd4r_info$def to i64
  %lndcR = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lndcQ, i64* %lndcR, !tbaa !1
  store i64 %R1_Arg, i64* %R2_Var
  %lndcS = bitcast i8* @integerzmgmp_GHCziIntegerziType_integerToInt_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndcT = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndcS( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lndcT, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cd4r_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd4r_info$def to i8*)
define internal ghccc void @cd4r_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd4r_info$def to i64)),i64 176), i64 67, i64 1059933504143392}>
{
cd4r:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %lsb7L = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %lndcU = getelementptr inbounds i64, i64* %Sp_Arg, i32 1
  %lndcV = bitcast i64* %lndcU to i64*
  %lndcW = load i64, i64* %lndcV, !tbaa !1
  %lndcX = bitcast i8* @ghczuwrapperZC23ZCbaseZCSystemziPosixziInternalsZClseek to i64 (i64, i64, i64)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lndcY = call ccc i64 (i64, i64, i64) %lndcX( i64 %lndcW, i64 %R1_Arg, i64 0 ) nounwind
  store i64 %lndcY, i64* %lsb7L
  %lndcZ = load i64, i64* %lsb7L
  switch i64 %lndcZ, label %ud6C [i64 -1, label %cd6x]
ud6C:
  %lndd0 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd4C_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndd0( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cd6x:
  %lndd1 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd6w_info$def to i64
  %lndd2 = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lndd1, i64* %lndd2, !tbaa !1
  %lndd3 = ptrtoint i8* @unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_fdSeek2_closure to i64
  store i64 %lndd3, i64* %R2_Var
  %lndd4 = bitcast i8* @base_ForeignziCziError_throwErrno1_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndd5 = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndd4( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lndd5, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cd6w_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd6w_info$def to i8*)
define internal ghccc void @cd6w_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd6w_info$def to i64)),i64 256), i64 67, i64 966367641632}>
{
cd6w:
  %lndd6 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd4C_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndd6( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cd4C_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd4C_info$def to i8*)
define internal ghccc void @cd4C_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd4C_info$def to i64)),i64 256), i64 67, i64 966367641632}>
{
cd4C:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb7O = alloca i64, i32 1
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %lndd7 = load i64*, i64** %Hp_Var
  %lndd8 = getelementptr inbounds i64, i64* %lndd7, i32 3
  %lndd9 = ptrtoint i64* %lndd8 to i64
  %lndda = inttoptr i64 %lndd9 to i64*
  store i64* %lndda, i64** %Hp_Var
  %lnddb = load i64*, i64** %Hp_Var
  %lnddc = ptrtoint i64* %lnddb to i64
  %lnddd = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lndde = bitcast i64* %lnddd to i64*
  %lnddf = load i64, i64* %lndde, !tbaa !4
  %lnddg = icmp ugt i64 %lnddc, %lnddf
  br i1 %lnddg, label %cd69, label %cd68
cd68:
  %lnddi = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb7W_info$def to i64
  %lnddh = load i64*, i64** %Hp_Var
  %lnddj = getelementptr inbounds i64, i64* %lnddh, i32 -2
  store i64 %lnddi, i64* %lnddj, !tbaa !2
  %lnddl = getelementptr inbounds i64, i64* %Sp_Arg, i32 1
  %lnddm = bitcast i64* %lnddl to i64*
  %lnddn = load i64, i64* %lnddm, !tbaa !1
  %lnddk = load i64*, i64** %Hp_Var
  %lnddo = getelementptr inbounds i64, i64* %lnddk, i32 -1
  store i64 %lnddn, i64* %lnddo, !tbaa !2
  %lnddp = getelementptr inbounds i64, i64* %Sp_Arg, i32 2
  %lnddq = bitcast i64* %lnddp to i64*
  %lnddr = load i64, i64* %lnddq, !tbaa !1
  %lndds = add i64 %lnddr, 16
  store i64 %lndds, i64* %lsb7O
  %lnddu = load i64, i64* %lsb7O
  %lnddt = load i64*, i64** %Hp_Var
  %lnddv = getelementptr inbounds i64, i64* %lnddt, i32 0
  store i64 %lnddu, i64* %lnddv, !tbaa !2
  %lnddw = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd5a_info$def to i64
  %lnddx = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lnddw, i64* %lnddx, !tbaa !1
  %lnddz = load i64*, i64** %Hp_Var
  %lnddA = ptrtoint i64* %lnddz to i64
  %lnddB = add i64 %lnddA, -15
  store i64 %lnddB, i64* %R4_Var
  %lnddC = ptrtoint i8* @unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_fdRead8_closure to i64
  store i64 %lnddC, i64* %R3_Var
  %lnddD = ptrtoint i8* @unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_fdRead9_closure to i64
  %lnddE = add i64 %lnddD, 1
  store i64 %lnddE, i64* %R2_Var
  %lnddF = load i64, i64* %lsb7O
  %lnddG = getelementptr inbounds i64, i64* %Sp_Arg, i32 1
  store i64 %lnddF, i64* %lnddG, !tbaa !1
  %lnddH = bitcast i8* @base_ForeignziCziError_throwErrnoIfMinus1Retry2_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnddI = load i64*, i64** %Hp_Var
  %lnddJ = load i64, i64* %R2_Var
  %lnddK = load i64, i64* %R3_Var
  %lnddL = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnddH( i64* %Base_Arg, i64* %Sp_Arg, i64* %lnddI, i64 %R1_Arg, i64 %lnddJ, i64 %lnddK, i64 %lnddL, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cd69:
  %lnddM = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lnddM, !tbaa !4
  %lnddN = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd4C_info$def to i64
  %lnddO = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lnddN, i64* %lnddO, !tbaa !1
  %lnddP = bitcast i8* @stg_gc_noregs to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnddQ = load i64*, i64** %Hp_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnddP( i64* %Base_Arg, i64* %Sp_Arg, i64* %lnddQ, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cd5a_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd5a_info$def to i8*)
define internal ghccc void @cd5a_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd5a_info$def to i64)),i64 256), i64 67, i64 141733920800}>
{
cd5a:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnddR = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd5c_info$def to i64
  %lnddS = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lnddR, i64* %lnddS, !tbaa !1
  %lnddT = load i64, i64* %R1_Var
  store i64 %lnddT, i64* %R1_Var
  %lnddU = load i64, i64* %R1_Var
  %lnddV = and i64 %lnddU, 7
  %lnddW = icmp ne i64 %lnddV, 0
  br i1 %lnddW, label %ud6D, label %cd5d
cd5d:
  %lnddY = load i64, i64* %R1_Var
  %lnddZ = inttoptr i64 %lnddY to i64*
  %lnde0 = load i64, i64* %lnddZ, !tbaa !3
  %lnde1 = inttoptr i64 %lnde0 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnde2 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnde1( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnde2, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ud6D:
  %lnde3 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd5c_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnde4 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnde3( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnde4, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cd5c_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd5c_info$def to i8*)
define internal ghccc void @cd5c_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd5c_info$def to i64)),i64 256), i64 67, i64 141733920800}>
{
cd5c:
  %lsb82 = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lsb7F = alloca i64, i32 1
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %lsb87 = alloca i64, i32 1
  %lsb8a = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnde7 = load i64, i64* %R1_Var
  %lnde8 = add i64 %lnde7, 7
  %lnde9 = inttoptr i64 %lnde8 to i64*
  %lndea = load i64, i64* %lnde9, !tbaa !3
  store i64 %lndea, i64* %lsb82
  %lndeb = load i64, i64* %lsb82
  switch i64 %lndeb, label %cd6e [i64 4096, label %cd6i]
cd6e:
  %lndec = load i64, i64* %lsb82
  store i64 %lndec, i64* %R3_Var
  %lnded = load i64*, i64** %Sp_Var
  %lndee = getelementptr inbounds i64, i64* %lnded, i32 3
  %lndef = bitcast i64* %lndee to i64*
  %lndeg = load i64, i64* %lndef, !tbaa !1
  store i64 %lndeg, i64* %R2_Var
  %lndeh = load i64*, i64** %Sp_Var
  %lndei = getelementptr inbounds i64, i64* %lndeh, i32 4
  %lndej = ptrtoint i64* %lndei to i64
  %lndek = inttoptr i64 %lndej to i64*
  store i64* %lndek, i64** %Sp_Var
  %lndel = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_readzudisk3_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndem = load i64*, i64** %Sp_Var
  %lnden = load i64, i64* %R1_Var
  %lndeo = load i64, i64* %R2_Var
  %lndep = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndel( i64* %Base_Arg, i64* %lndem, i64* %Hp_Arg, i64 %lnden, i64 %lndeo, i64 %lndep, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cd6i:
  %lndeq = load i64*, i64** %Sp_Var
  %lnder = getelementptr inbounds i64, i64* %lndeq, i32 2
  %lndes = bitcast i64* %lnder to i64*
  %lndet = load i64, i64* %lndes, !tbaa !1
  store i64 %lndet, i64* %lsb7F
  %lndeu = load i64*, i64** %Sp_Var
  %lndev = getelementptr inbounds i64, i64* %lndeu, i32 1
  %lndew = bitcast i64* %lndev to i64*
  %lndex = load i64, i64* %lndew, !tbaa !1
  %lndey = inttoptr i64 %lndex to i8*
  %lndez = bitcast i8* @integer_gmp_rscan_nzbyte to i64 (i8*, i64, i64)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lndeA = call ccc i64 (i8*, i64, i64) %lndez( i8* %lndey, i64 0, i64 4096 ) nounwind
  store i64 %lndeA, i64* %lsb87
  %lndeB = load i64, i64* %lsb87
  switch i64 %lndeB, label %cd6m [i64 0, label %cd6q]
cd6m:
  %lndeD = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd5G_info$def to i64
  %lndeC = load i64*, i64** %Sp_Var
  %lndeE = getelementptr inbounds i64, i64* %lndeC, i32 0
  store i64 %lndeD, i64* %lndeE, !tbaa !1
  %lndeF = load i64, i64* %lsb87
  %lndeG = add i64 %lndeF, 7
  store i64 %lndeG, i64* %lsb8a
  %lndeH = load i64, i64* %lsb8a
  %lndeI = load i64, i64* %lsb8a
  %lndeJ = ashr i64 %lndeI, 63
  %lndeK = and i64 %lndeJ, 7
  %lndeL = add i64 %lndeH, %lndeK
  %lndeM = ashr i64 %lndeL, 3
  %lndeN = shl i64 %lndeM, 3
  store i64 %lndeN, i64* %R1_Var
  %lndeP = load i64, i64* %lsb87
  %lndeO = load i64*, i64** %Sp_Var
  %lndeQ = getelementptr inbounds i64, i64* %lndeO, i32 3
  store i64 %lndeP, i64* %lndeQ, !tbaa !1
  %lndeR = bitcast i8* @stg_newByteArrayzh to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndeS = load i64*, i64** %Sp_Var
  %lndeT = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndeR( i64* %Base_Arg, i64* %lndeS, i64* %Hp_Arg, i64 %lndeT, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cd6q:
  %lndeU = ptrtoint %Disk_readzudisk2_closure_struct* @Disk_readzudisk2_closure$def to i64
  %lndeV = add i64 %lndeU, 1
  store i64 %lndeV, i64* %R1_Var
  %lndeW = load i64*, i64** %Sp_Var
  %lndeX = getelementptr inbounds i64, i64* %lndeW, i32 4
  %lndeY = ptrtoint i64* %lndeX to i64
  %lndeZ = inttoptr i64 %lndeY to i64*
  store i64* %lndeZ, i64** %Sp_Var
  %lndf0 = load i64*, i64** %Sp_Var
  %lndf1 = getelementptr inbounds i64, i64* %lndf0, i32 0
  %lndf2 = bitcast i64* %lndf1 to i64*
  %lndf3 = load i64, i64* %lndf2, !tbaa !1
  %lndf4 = inttoptr i64 %lndf3 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndf5 = load i64*, i64** %Sp_Var
  %lndf6 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndf4( i64* %Base_Arg, i64* %lndf5, i64* %Hp_Arg, i64 %lndf6, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cd5G_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cd5G_info$def to i8*)
define internal ghccc void @cd5G_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 323, i64 32}>
{
cd5G:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb7F = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lndf7 = load i64*, i64** %Hp_Var
  %lndf8 = getelementptr inbounds i64, i64* %lndf7, i32 5
  %lndf9 = ptrtoint i64* %lndf8 to i64
  %lndfa = inttoptr i64 %lndf9 to i64*
  store i64* %lndfa, i64** %Hp_Var
  %lndfb = load i64*, i64** %Hp_Var
  %lndfc = ptrtoint i64* %lndfb to i64
  %lndfd = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lndfe = bitcast i64* %lndfd to i64*
  %lndff = load i64, i64* %lndfe, !tbaa !4
  %lndfg = icmp ugt i64 %lndfc, %lndff
  br i1 %lndfg, label %cd6p, label %cd6o
cd6o:
  %lndfh = load i64*, i64** %Sp_Var
  %lndfi = getelementptr inbounds i64, i64* %lndfh, i32 2
  %lndfj = bitcast i64* %lndfi to i64*
  %lndfk = load i64, i64* %lndfj, !tbaa !1
  store i64 %lndfk, i64* %lsb7F
  %lndfm = load i64, i64* %R1_Var
  %lndfn = add i64 %lndfm, 16
  %lndfo = inttoptr i64 %lndfn to i8*
  %lndfp = load i64*, i64** %Sp_Var
  %lndfq = getelementptr inbounds i64, i64* %lndfp, i32 1
  %lndfr = bitcast i64* %lndfq to i64*
  %lndfs = load i64, i64* %lndfr, !tbaa !1
  %lndft = inttoptr i64 %lndfs to i8*
  %lndfu = load i64*, i64** %Sp_Var
  %lndfv = getelementptr inbounds i64, i64* %lndfu, i32 3
  %lndfw = bitcast i64* %lndfv to i64*
  %lndfx = load i64, i64* %lndfw, !tbaa !1
  %lndfy = bitcast i8* @integer_gmp_mpn_import to void (i8*, i8*, i64, i64, i64)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  call ccc void (i8*, i8*, i64, i64, i64) %lndfy( i8* %lndfo, i8* %lndft, i64 0, i64 %lndfx, i64 0 ) nounwind
  %lndfA = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb8n_info$def to i64
  %lndfz = load i64*, i64** %Hp_Var
  %lndfB = getelementptr inbounds i64, i64* %lndfz, i32 -4
  store i64 %lndfA, i64* %lndfB, !tbaa !2
  %lndfD = load i64, i64* %R1_Var
  %lndfC = load i64*, i64** %Hp_Var
  %lndfE = getelementptr inbounds i64, i64* %lndfC, i32 -2
  store i64 %lndfD, i64* %lndfE, !tbaa !2
  %lndfG = ptrtoint i8* @Word_W_con_info to i64
  %lndfF = load i64*, i64** %Hp_Var
  %lndfH = getelementptr inbounds i64, i64* %lndfF, i32 -1
  store i64 %lndfG, i64* %lndfH, !tbaa !2
  %lndfJ = load i64*, i64** %Hp_Var
  %lndfK = getelementptr inbounds i64, i64* %lndfJ, i32 -4
  %lndfL = ptrtoint i64* %lndfK to i64
  %lndfI = load i64*, i64** %Hp_Var
  %lndfM = getelementptr inbounds i64, i64* %lndfI, i32 0
  store i64 %lndfL, i64* %lndfM, !tbaa !2
  %lndfO = load i64*, i64** %Hp_Var
  %lndfP = ptrtoint i64* %lndfO to i64
  %lndfQ = add i64 %lndfP, -7
  store i64 %lndfQ, i64* %R1_Var
  %lndfR = load i64*, i64** %Sp_Var
  %lndfS = getelementptr inbounds i64, i64* %lndfR, i32 4
  %lndfT = ptrtoint i64* %lndfS to i64
  %lndfU = inttoptr i64 %lndfT to i64*
  store i64* %lndfU, i64** %Sp_Var
  %lndfV = load i64*, i64** %Sp_Var
  %lndfW = getelementptr inbounds i64, i64* %lndfV, i32 0
  %lndfX = bitcast i64* %lndfW to i64*
  %lndfY = load i64, i64* %lndfX, !tbaa !1
  %lndfZ = inttoptr i64 %lndfY to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndg0 = load i64*, i64** %Sp_Var
  %lndg1 = load i64*, i64** %Hp_Var
  %lndg2 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndfZ( i64* %Base_Arg, i64* %lndg0, i64* %lndg1, i64 %lndg2, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cd6p:
  %lndg3 = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 40, i64* %lndg3, !tbaa !4
  %lndg4 = load i64, i64* %R1_Var
  store i64 %lndg4, i64* %R1_Var
  %lndg5 = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndg6 = load i64*, i64** %Sp_Var
  %lndg7 = load i64*, i64** %Hp_Var
  %lndg8 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndg5( i64* %Base_Arg, i64* %lndg6, i64* %lndg7, i64 %lndg8, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_readzudisk1_closure_struct = type <{i64, i64}>
@Disk_readzudisk1_closure$def = internal global %Disk_readzudisk1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_readzudisk1_info$def to i64), i64 0}>
@Disk_readzudisk1_closure = alias i8* bitcast (%Disk_readzudisk1_closure_struct* @Disk_readzudisk1_closure$def to i8*)
@Disk_readzudisk1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_readzudisk1_info$def to i8*)
define ghccc void @Disk_readzudisk1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_readzudisk1_info$def to i64)),i64 280), i64 12884901903, i64 0, i64 141733920783}>
{
cdgk:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lndgr = load i64*, i64** %Sp_Var
  %lndgs = getelementptr inbounds i64, i64* %lndgr, i32 -2
  %lndgt = ptrtoint i64* %lndgs to i64
  %lndgu = icmp ult i64 %lndgt, %SpLim_Arg
  br i1 %lndgu, label %cdgl, label %cdgm
cdgm:
  %lndgw = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdgh_info$def to i64
  %lndgv = load i64*, i64** %Sp_Var
  %lndgx = getelementptr inbounds i64, i64* %lndgv, i32 -2
  store i64 %lndgw, i64* %lndgx, !tbaa !1
  %lndgy = load i64, i64* %R2_Var
  store i64 %lndgy, i64* %R1_Var
  %lndgA = load i64, i64* %R3_Var
  %lndgz = load i64*, i64** %Sp_Var
  %lndgB = getelementptr inbounds i64, i64* %lndgz, i32 -1
  store i64 %lndgA, i64* %lndgB, !tbaa !1
  %lndgC = load i64*, i64** %Sp_Var
  %lndgD = getelementptr inbounds i64, i64* %lndgC, i32 -2
  %lndgE = ptrtoint i64* %lndgD to i64
  %lndgF = inttoptr i64 %lndgE to i64*
  store i64* %lndgF, i64** %Sp_Var
  %lndgG = load i64, i64* %R1_Var
  %lndgH = and i64 %lndgG, 7
  %lndgI = icmp ne i64 %lndgH, 0
  br i1 %lndgI, label %udgq, label %cdgi
cdgi:
  %lndgK = load i64, i64* %R1_Var
  %lndgL = inttoptr i64 %lndgK to i64*
  %lndgM = load i64, i64* %lndgL, !tbaa !3
  %lndgN = inttoptr i64 %lndgM to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndgO = load i64*, i64** %Sp_Var
  %lndgP = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndgN( i64* %Base_Arg, i64* %lndgO, i64* %Hp_Arg, i64 %lndgP, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
udgq:
  %lndgQ = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdgh_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndgR = load i64*, i64** %Sp_Var
  %lndgS = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndgQ( i64* %Base_Arg, i64* %lndgR, i64* %Hp_Arg, i64 %lndgS, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdgl:
  %lndgT = load i64, i64* %R3_Var
  store i64 %lndgT, i64* %R3_Var
  %lndgU = load i64, i64* %R2_Var
  store i64 %lndgU, i64* %R2_Var
  %lndgV = ptrtoint %Disk_readzudisk1_closure_struct* @Disk_readzudisk1_closure$def to i64
  store i64 %lndgV, i64* %R1_Var
  %lndgW = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lndgX = bitcast i64* %lndgW to i64*
  %lndgY = load i64, i64* %lndgX, !tbaa !4
  %lndgZ = inttoptr i64 %lndgY to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndh0 = load i64*, i64** %Sp_Var
  %lndh1 = load i64, i64* %R1_Var
  %lndh2 = load i64, i64* %R2_Var
  %lndh3 = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndgZ( i64* %Base_Arg, i64* %lndh0, i64* %Hp_Arg, i64 %lndh1, i64 %lndh2, i64 %lndh3, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdgh_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdgh_info$def to i8*)
define internal ghccc void @cdgh_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdgh_info$def to i64)),i64 280), i64 1, i64 4294967328}>
{
cdgh:
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lndh4 = load i64*, i64** %Sp_Var
  %lndh5 = getelementptr inbounds i64, i64* %lndh4, i32 1
  %lndh6 = bitcast i64* %lndh5 to i64*
  %lndh7 = load i64, i64* %lndh6, !tbaa !1
  store i64 %lndh7, i64* %R5_Var
  %lndh8 = add i64 %R1_Arg, 23
  %lndh9 = inttoptr i64 %lndh8 to i64*
  %lndha = load i64, i64* %lndh9, !tbaa !3
  store i64 %lndha, i64* %R4_Var
  %lndhb = add i64 %R1_Arg, 7
  %lndhc = inttoptr i64 %lndhb to i64*
  %lndhd = load i64, i64* %lndhc, !tbaa !3
  store i64 %lndhd, i64* %R3_Var
  %lndhe = add i64 %R1_Arg, 31
  %lndhf = inttoptr i64 %lndhe to i64*
  %lndhg = load i64, i64* %lndhf, !tbaa !3
  store i64 %lndhg, i64* %R2_Var
  %lndhh = load i64*, i64** %Sp_Var
  %lndhi = getelementptr inbounds i64, i64* %lndhh, i32 2
  %lndhj = ptrtoint i64* %lndhi to i64
  %lndhk = inttoptr i64 %lndhj to i64*
  store i64* %lndhk, i64** %Sp_Var
  %lndhl = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_zdwreadzudisk_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndhm = load i64*, i64** %Sp_Var
  %lndhn = load i64, i64* %R2_Var
  %lndho = load i64, i64* %R3_Var
  %lndhp = load i64, i64* %R4_Var
  %lndhq = load i64, i64* %R5_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndhl( i64* %Base_Arg, i64* %lndhm, i64* %Hp_Arg, i64 %R1_Arg, i64 %lndhn, i64 %lndho, i64 %lndhp, i64 %lndhq, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_readzudisk_closure_struct = type <{i64, i64}>
@Disk_readzudisk_closure$def = internal global %Disk_readzudisk_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_readzudisk_info$def to i64), i64 0}>
@Disk_readzudisk_closure = alias i8* bitcast (%Disk_readzudisk_closure_struct* @Disk_readzudisk_closure$def to i8*)
@Disk_readzudisk_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_readzudisk_info$def to i8*)
define ghccc void @Disk_readzudisk_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_readzudisk_info$def to i64)),i64 320), i64 12884901903, i64 0, i64 4294967311}>
{
cdhw:
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lndhz = load i64, i64* %R3_Var
  store i64 %lndhz, i64* %R3_Var
  %lndhA = load i64, i64* %R2_Var
  store i64 %lndhA, i64* %R2_Var
  %lndhB = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_readzudisk1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndhC = load i64, i64* %R2_Var
  %lndhD = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndhB( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lndhC, i64 %lndhD, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%rb0L_closure_struct = type <{i64, i64}>
@rb0L_closure$def = internal global %rb0L_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_Izh_static_info to i64), i64 94}>
@rb0L_closure = internal alias i8* bitcast (%rb0L_closure_struct* @rb0L_closure$def to i8*)
%rb0M_closure_struct = type <{i64, i64}>
@rb0M_closure$def = internal global %rb0M_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_Izh_static_info to i64), i64 26}>
@rb0M_closure = internal alias i8* bitcast (%rb0M_closure_struct* @rb0M_closure$def to i8*)
%rb0N_closure_struct = type <{i64, i64}>
@rb0N_closure$def = internal global %rb0N_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_Izh_static_info to i64), i64 57}>
@rb0N_closure = internal alias i8* bitcast (%rb0N_closure_struct* @rb0N_closure$def to i8*)
%rb0O_closure_struct = type <{i64, i64, i64, i64, i64, i64, i64, i64, i64}>
@rb0O_closure$def = internal global %rb0O_closure_struct<{i64 ptrtoint (i8* @base_GHCziStackziTypes_SrcLoc_static_info to i64), i64 ptrtoint (%rb0s_closure_struct* @rb0s_closure$def to i64), i64 ptrtoint (%rb0t_closure_struct* @rb0t_closure$def to i64), i64 ptrtoint (%rb0u_closure_struct* @rb0u_closure$def to i64), i64 add (i64 ptrtoint (%rb0L_closure_struct* @rb0L_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%rb0M_closure_struct* @rb0M_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%rb0L_closure_struct* @rb0L_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%rb0N_closure_struct* @rb0N_closure$def to i64),i64 1), i64 0}>
@rb0O_closure = internal alias i8* bitcast (%rb0O_closure_struct* @rb0O_closure$def to i8*)
%rb0P_closure_struct = type <{i64, i64, i64, i64, i64}>
@rb0P_closure$def = internal global %rb0P_closure_struct<{i64 ptrtoint (i8* @base_GHCziStackziTypes_PushCallStack_static_info to i64), i64 ptrtoint (%rb0y_closure_struct* @rb0y_closure$def to i64), i64 add (i64 ptrtoint (%rb0O_closure_struct* @rb0O_closure$def to i64),i64 1), i64 add (i64 ptrtoint (i8* @base_GHCziStackziTypes_EmptyCallStack_closure to i64),i64 1), i64 0}>
@rb0P_closure = internal alias i8* bitcast (%rb0P_closure_struct* @rb0P_closure$def to i8*)
%rb0Q_closure_struct = type <{i64, i64, i64, i64}>
%cdhQ_str_struct = type <{[24 x i8]}>
@rb0Q_closure$def = internal global %rb0Q_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rb0Q_info$def to i64), i64 0, i64 0, i64 0}>
@rb0Q_closure = internal alias i8* bitcast (%rb0Q_closure_struct* @rb0Q_closure$def to i8*)
@cdhQ_str$def = internal constant %cdhQ_str_struct<{[24 x i8] [i8 119, i8 114, i8 105, i8 116, i8 101, i8 95, i8 100, i8 105, i8 115, i8 107, i8 58, i8 32, i8 115, i8 104, i8 111, i8 114, i8 116, i8 32, i8 118, i8 97, i8 108, i8 117, i8 101, i8 0]}>
@cdhQ_str = internal alias i8* bitcast (%cdhQ_str_struct* @cdhQ_str$def to i8*)
@rb0Q_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rb0Q_info$def to i8*)
define internal ghccc void @rb0Q_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 22}>
{
cdhR:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %lcdhN = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lndhU = load i64*, i64** %Sp_Var
  %lndhV = getelementptr inbounds i64, i64* %lndhU, i32 -2
  %lndhW = ptrtoint i64* %lndhV to i64
  %lndhX = icmp ult i64 %lndhW, %SpLim_Arg
  br i1 %lndhX, label %cdhS, label %cdhT
cdhT:
  %lndhY = ptrtoint i64* %Base_Arg to i64
  %lndhZ = inttoptr i64 %lndhY to i8*
  %lndi0 = load i64, i64* %R1_Var
  %lndi1 = inttoptr i64 %lndi0 to i8*
  %lndi2 = bitcast i8* @newCAF to i8* (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lndi3 = call ccc i8* (i8*, i8*) %lndi2( i8* %lndhZ, i8* %lndi1 ) nounwind
  %lndi4 = ptrtoint i8* %lndi3 to i64
  store i64 %lndi4, i64* %lcdhN
  %lndi5 = load i64, i64* %lcdhN
  %lndi6 = icmp eq i64 %lndi5, 0
  br i1 %lndi6, label %cdhP, label %cdhO
cdhO:
  %lndi8 = ptrtoint i8* @stg_bh_upd_frame_info to i64
  %lndi7 = load i64*, i64** %Sp_Var
  %lndi9 = getelementptr inbounds i64, i64* %lndi7, i32 -2
  store i64 %lndi8, i64* %lndi9, !tbaa !1
  %lndib = load i64, i64* %lcdhN
  %lndia = load i64*, i64** %Sp_Var
  %lndic = getelementptr inbounds i64, i64* %lndia, i32 -1
  store i64 %lndib, i64* %lndic, !tbaa !1
  %lndid = ptrtoint %cdhQ_str_struct* @cdhQ_str$def to i64
  store i64 %lndid, i64* %R2_Var
  %lndie = load i64*, i64** %Sp_Var
  %lndif = getelementptr inbounds i64, i64* %lndie, i32 -2
  %lndig = ptrtoint i64* %lndif to i64
  %lndih = inttoptr i64 %lndig to i64*
  store i64* %lndih, i64** %Sp_Var
  %lndii = bitcast i8* @ghczmprim_GHCziCString_unpackCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndij = load i64*, i64** %Sp_Var
  %lndik = load i64, i64* %R1_Var
  %lndil = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndii( i64* %Base_Arg, i64* %lndij, i64* %Hp_Arg, i64 %lndik, i64 %lndil, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdhP:
  %lndin = load i64, i64* %R1_Var
  %lndio = inttoptr i64 %lndin to i64*
  %lndip = load i64, i64* %lndio, !tbaa !3
  %lndiq = inttoptr i64 %lndip to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndir = load i64*, i64** %Sp_Var
  %lndis = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndiq( i64* %Base_Arg, i64* %lndir, i64* %Hp_Arg, i64 %lndis, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdhS:
  %lndit = load i64, i64* %R1_Var
  store i64 %lndit, i64* %R1_Var
  %lndiu = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lndiv = bitcast i64* %lndiu to i64*
  %lndiw = load i64, i64* %lndiv, !tbaa !4
  %lndix = inttoptr i64 %lndiw to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndiy = load i64*, i64** %Sp_Var
  %lndiz = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndix( i64* %Base_Arg, i64* %lndiy, i64* %Hp_Arg, i64 %lndiz, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%rb0R_closure_struct = type <{i64, i64, i64, i64}>
@rb0R_closure$def = internal global %rb0R_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rb0R_info$def to i64), i64 0, i64 0, i64 0}>
@rb0R_closure = internal alias i8* bitcast (%rb0R_closure_struct* @rb0R_closure$def to i8*)
@rb0R_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rb0R_info$def to i8*)
define internal ghccc void @rb0R_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rb0R_info$def to i64)),i64 328), i64 0, i64 30064771094}>
{
cdiH:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %lcdiE = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lndiK = load i64*, i64** %Sp_Var
  %lndiL = getelementptr inbounds i64, i64* %lndiK, i32 -2
  %lndiM = ptrtoint i64* %lndiL to i64
  %lndiN = icmp ult i64 %lndiM, %SpLim_Arg
  br i1 %lndiN, label %cdiI, label %cdiJ
cdiJ:
  %lndiO = ptrtoint i64* %Base_Arg to i64
  %lndiP = inttoptr i64 %lndiO to i8*
  %lndiQ = load i64, i64* %R1_Var
  %lndiR = inttoptr i64 %lndiQ to i8*
  %lndiS = bitcast i8* @newCAF to i8* (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lndiT = call ccc i8* (i8*, i8*) %lndiS( i8* %lndiP, i8* %lndiR ) nounwind
  %lndiU = ptrtoint i8* %lndiT to i64
  store i64 %lndiU, i64* %lcdiE
  %lndiV = load i64, i64* %lcdiE
  %lndiW = icmp eq i64 %lndiV, 0
  br i1 %lndiW, label %cdiG, label %cdiF
cdiF:
  %lndiY = ptrtoint i8* @stg_bh_upd_frame_info to i64
  %lndiX = load i64*, i64** %Sp_Var
  %lndiZ = getelementptr inbounds i64, i64* %lndiX, i32 -2
  store i64 %lndiY, i64* %lndiZ, !tbaa !1
  %lndj1 = load i64, i64* %lcdiE
  %lndj0 = load i64*, i64** %Sp_Var
  %lndj2 = getelementptr inbounds i64, i64* %lndj0, i32 -1
  store i64 %lndj1, i64* %lndj2, !tbaa !1
  %lndj3 = ptrtoint %rb0Q_closure_struct* @rb0Q_closure$def to i64
  store i64 %lndj3, i64* %R3_Var
  %lndj4 = ptrtoint %rb0P_closure_struct* @rb0P_closure$def to i64
  %lndj5 = add i64 %lndj4, 2
  store i64 %lndj5, i64* %R2_Var
  %lndj6 = load i64*, i64** %Sp_Var
  %lndj7 = getelementptr inbounds i64, i64* %lndj6, i32 -2
  %lndj8 = ptrtoint i64* %lndj7 to i64
  %lndj9 = inttoptr i64 %lndj8 to i64*
  store i64* %lndj9, i64** %Sp_Var
  %lndja = bitcast i8* @base_GHCziErr_error_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndjb = load i64*, i64** %Sp_Var
  %lndjc = load i64, i64* %R1_Var
  %lndjd = load i64, i64* %R2_Var
  %lndje = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndja( i64* %Base_Arg, i64* %lndjb, i64* %Hp_Arg, i64 %lndjc, i64 %lndjd, i64 %lndje, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdiG:
  %lndjg = load i64, i64* %R1_Var
  %lndjh = inttoptr i64 %lndjg to i64*
  %lndji = load i64, i64* %lndjh, !tbaa !3
  %lndjj = inttoptr i64 %lndji to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndjk = load i64*, i64** %Sp_Var
  %lndjl = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndjj( i64* %Base_Arg, i64* %lndjk, i64* %Hp_Arg, i64 %lndjl, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdiI:
  %lndjm = load i64, i64* %R1_Var
  store i64 %lndjm, i64* %R1_Var
  %lndjn = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lndjo = bitcast i64* %lndjn to i64*
  %lndjp = load i64, i64* %lndjo, !tbaa !4
  %lndjq = inttoptr i64 %lndjp to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndjr = load i64*, i64** %Sp_Var
  %lndjs = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndjq( i64* %Base_Arg, i64* %lndjr, i64* %Hp_Arg, i64 %lndjs, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%rb0S_closure_struct = type <{i64, i64}>
%cdko_str_struct = type <{[4 x i8]}>
%cdl6_str_struct = type <{[26 x i8]}>
@rb0S_closure$def = internal global %rb0S_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rb0S_info$def to i64), i64 0}>
@rb0S_closure = internal alias i8* bitcast (%rb0S_closure_struct* @rb0S_closure$def to i8*)
@cdko_str$def = internal constant %cdko_str_struct<{[4 x i8] [i8 32, i8 64, i8 32, i8 0]}>
@cdko_str = internal alias i8* bitcast (%cdko_str_struct* @cdko_str$def to i8*)
@cdl6_str$def = internal constant %cdl6_str_struct<{[26 x i8] [i8 119, i8 114, i8 105, i8 116, i8 101, i8 95, i8 100, i8 105, i8 115, i8 107, i8 58, i8 32, i8 115, i8 104, i8 111, i8 114, i8 116, i8 32, i8 119, i8 114, i8 105, i8 116, i8 101, i8 58, i8 32, i8 0]}>
@cdl6_str = internal alias i8* bitcast (%cdl6_str_struct* @cdl6_str$def to i8*)
@sb8M_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb8M_info$def to i8*)
define internal ghccc void @sb8M_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb8M_info$def to i64)),i64 240), i64 1, i64 4294967313}>
{
cdki:
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lndlh = load i64*, i64** %Sp_Var
  %lndli = getelementptr inbounds i64, i64* %lndlh, i32 -3
  %lndlj = ptrtoint i64* %lndli to i64
  %lndlk = icmp ult i64 %lndlj, %SpLim_Arg
  br i1 %lndlk, label %cdkj, label %cdkk
cdkk:
  %lndlm = ptrtoint i8* @stg_upd_frame_info to i64
  %lndll = load i64*, i64** %Sp_Var
  %lndln = getelementptr inbounds i64, i64* %lndll, i32 -2
  store i64 %lndlm, i64* %lndln, !tbaa !1
  %lndlp = load i64, i64* %R1_Var
  %lndlo = load i64*, i64** %Sp_Var
  %lndlq = getelementptr inbounds i64, i64* %lndlo, i32 -1
  store i64 %lndlp, i64* %lndlq, !tbaa !1
  %lndls = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdkf_info$def to i64
  %lndlr = load i64*, i64** %Sp_Var
  %lndlt = getelementptr inbounds i64, i64* %lndlr, i32 -3
  store i64 %lndls, i64* %lndlt, !tbaa !1
  %lndlu = ptrtoint i8* @ghczmprim_GHCziTypes_ZMZN_closure to i64
  %lndlv = add i64 %lndlu, 1
  store i64 %lndlv, i64* %R4_Var
  %lndly = load i64, i64* %R1_Var
  %lndlz = add i64 %lndly, 16
  %lndlA = inttoptr i64 %lndlz to i64*
  %lndlB = load i64, i64* %lndlA, !tbaa !3
  store i64 %lndlB, i64* %R3_Var
  store i64 0, i64* %R2_Var
  %lndlC = load i64*, i64** %Sp_Var
  %lndlD = getelementptr inbounds i64, i64* %lndlC, i32 -3
  %lndlE = ptrtoint i64* %lndlD to i64
  %lndlF = inttoptr i64 %lndlE to i64*
  store i64* %lndlF, i64** %Sp_Var
  %lndlG = bitcast i8* @base_GHCziShow_zdwzdcshowsPrec3_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndlH = load i64*, i64** %Sp_Var
  %lndlI = load i64, i64* %R1_Var
  %lndlJ = load i64, i64* %R2_Var
  %lndlK = load i64, i64* %R3_Var
  %lndlL = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndlG( i64* %Base_Arg, i64* %lndlH, i64* %Hp_Arg, i64 %lndlI, i64 %lndlJ, i64 %lndlK, i64 %lndlL, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdkj:
  %lndlM = load i64, i64* %R1_Var
  store i64 %lndlM, i64* %R1_Var
  %lndlN = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lndlO = bitcast i64* %lndlN to i64*
  %lndlP = load i64, i64* %lndlO, !tbaa !4
  %lndlQ = inttoptr i64 %lndlP to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndlR = load i64*, i64** %Sp_Var
  %lndlS = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndlQ( i64* %Base_Arg, i64* %lndlR, i64* %Hp_Arg, i64 %lndlS, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdkf_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdkf_info$def to i8*)
define internal ghccc void @cdkf_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
cdkf:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lndlT = load i64*, i64** %Hp_Var
  %lndlU = getelementptr inbounds i64, i64* %lndlT, i32 3
  %lndlV = ptrtoint i64* %lndlU to i64
  %lndlW = inttoptr i64 %lndlV to i64*
  store i64* %lndlW, i64** %Hp_Var
  %lndlX = load i64*, i64** %Hp_Var
  %lndlY = ptrtoint i64* %lndlX to i64
  %lndlZ = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lndm0 = bitcast i64* %lndlZ to i64*
  %lndm1 = load i64, i64* %lndm0, !tbaa !4
  %lndm2 = icmp ugt i64 %lndlY, %lndm1
  br i1 %lndm2, label %cdkn, label %cdkm
cdkm:
  %lndm4 = ptrtoint i8* @ghczmprim_GHCziTypes_ZC_con_info to i64
  %lndm3 = load i64*, i64** %Hp_Var
  %lndm5 = getelementptr inbounds i64, i64* %lndm3, i32 -2
  store i64 %lndm4, i64* %lndm5, !tbaa !2
  %lndm7 = load i64, i64* %R1_Var
  %lndm6 = load i64*, i64** %Hp_Var
  %lndm8 = getelementptr inbounds i64, i64* %lndm6, i32 -1
  store i64 %lndm7, i64* %lndm8, !tbaa !2
  %lndma = load i64, i64* %R2_Var
  %lndm9 = load i64*, i64** %Hp_Var
  %lndmb = getelementptr inbounds i64, i64* %lndm9, i32 0
  store i64 %lndma, i64* %lndmb, !tbaa !2
  %lndmd = load i64*, i64** %Hp_Var
  %lndme = ptrtoint i64* %lndmd to i64
  %lndmf = add i64 %lndme, -14
  store i64 %lndmf, i64* %R1_Var
  %lndmg = load i64*, i64** %Sp_Var
  %lndmh = getelementptr inbounds i64, i64* %lndmg, i32 1
  %lndmi = ptrtoint i64* %lndmh to i64
  %lndmj = inttoptr i64 %lndmi to i64*
  store i64* %lndmj, i64** %Sp_Var
  %lndmk = load i64*, i64** %Sp_Var
  %lndml = getelementptr inbounds i64, i64* %lndmk, i32 0
  %lndmm = bitcast i64* %lndml to i64*
  %lndmn = load i64, i64* %lndmm, !tbaa !1
  %lndmo = inttoptr i64 %lndmn to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndmp = load i64*, i64** %Sp_Var
  %lndmq = load i64*, i64** %Hp_Var
  %lndmr = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndmo( i64* %Base_Arg, i64* %lndmp, i64* %lndmq, i64 %lndmr, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdkn:
  %lndms = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lndms, !tbaa !4
  %lndmt = load i64, i64* %R2_Var
  store i64 %lndmt, i64* %R2_Var
  %lndmu = load i64, i64* %R1_Var
  store i64 %lndmu, i64* %R1_Var
  %lndmv = bitcast i8* @stg_gc_pp to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndmw = load i64*, i64** %Sp_Var
  %lndmx = load i64*, i64** %Hp_Var
  %lndmy = load i64, i64* %R1_Var
  %lndmz = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndmv( i64* %Base_Arg, i64* %lndmw, i64* %lndmx, i64 %lndmy, i64 %lndmz, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@sb8N_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb8N_info$def to i8*)
define internal ghccc void @sb8N_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb8N_info$def to i64)),i64 240), i64 1, i64 4294967313}>
{
cdkp:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb8C = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lndmA = load i64*, i64** %Hp_Var
  %lndmB = getelementptr inbounds i64, i64* %lndmA, i32 3
  %lndmC = ptrtoint i64* %lndmB to i64
  %lndmD = inttoptr i64 %lndmC to i64*
  store i64* %lndmD, i64** %Hp_Var
  %lndmE = load i64*, i64** %Hp_Var
  %lndmF = ptrtoint i64* %lndmE to i64
  %lndmG = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lndmH = bitcast i64* %lndmG to i64*
  %lndmI = load i64, i64* %lndmH, !tbaa !4
  %lndmJ = icmp ugt i64 %lndmF, %lndmI
  br i1 %lndmJ, label %cdkt, label %cdks
cdks:
  %lndmM = load i64, i64* %R1_Var
  %lndmN = add i64 %lndmM, 16
  %lndmO = inttoptr i64 %lndmN to i64*
  %lndmP = load i64, i64* %lndmO, !tbaa !3
  store i64 %lndmP, i64* %lsb8C
  %lndmR = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb8M_info$def to i64
  %lndmQ = load i64*, i64** %Hp_Var
  %lndmS = getelementptr inbounds i64, i64* %lndmQ, i32 -2
  store i64 %lndmR, i64* %lndmS, !tbaa !2
  %lndmU = load i64, i64* %lsb8C
  %lndmT = load i64*, i64** %Hp_Var
  %lndmV = getelementptr inbounds i64, i64* %lndmT, i32 0
  store i64 %lndmU, i64* %lndmV, !tbaa !2
  %lndmW = load i64*, i64** %Hp_Var
  %lndmX = getelementptr inbounds i64, i64* %lndmW, i32 -2
  %lndmY = ptrtoint i64* %lndmX to i64
  store i64 %lndmY, i64* %R3_Var
  %lndmZ = ptrtoint %cdko_str_struct* @cdko_str$def to i64
  store i64 %lndmZ, i64* %R2_Var
  %lndn0 = bitcast i8* @ghczmprim_GHCziCString_unpackAppendCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndn1 = load i64*, i64** %Hp_Var
  %lndn2 = load i64, i64* %R1_Var
  %lndn3 = load i64, i64* %R2_Var
  %lndn4 = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndn0( i64* %Base_Arg, i64* %Sp_Arg, i64* %lndn1, i64 %lndn2, i64 %lndn3, i64 %lndn4, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdkt:
  %lndn5 = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lndn5, !tbaa !4
  %lndn6 = load i64, i64* %R1_Var
  store i64 %lndn6, i64* %R1_Var
  %lndn7 = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lndn8 = bitcast i64* %lndn7 to i64*
  %lndn9 = load i64, i64* %lndn8, !tbaa !4
  %lndna = inttoptr i64 %lndn9 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndnb = load i64*, i64** %Hp_Var
  %lndnc = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndna( i64* %Base_Arg, i64* %Sp_Arg, i64* %lndnb, i64 %lndnc, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@sb8Y_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb8Y_info$def to i8*)
define internal ghccc void @sb8Y_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb8Y_info$def to i64)),i64 240), i64 4294967297, i64 12884901908}>
{
cdku:
  %lsb8C = alloca i64, i32 1
  %lsb8D = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lndnd = load i64*, i64** %Sp_Var
  %lndne = getelementptr inbounds i64, i64* %lndnd, i32 -5
  %lndnf = ptrtoint i64* %lndne to i64
  %lndng = icmp ult i64 %lndnf, %SpLim_Arg
  br i1 %lndng, label %cdkv, label %cdkZ
cdkZ:
  %lndni = ptrtoint i8* @stg_upd_frame_info to i64
  %lndnh = load i64*, i64** %Sp_Var
  %lndnj = getelementptr inbounds i64, i64* %lndnh, i32 -2
  store i64 %lndni, i64* %lndnj, !tbaa !1
  %lndnl = load i64, i64* %R1_Var
  %lndnk = load i64*, i64** %Sp_Var
  %lndnm = getelementptr inbounds i64, i64* %lndnk, i32 -1
  store i64 %lndnl, i64* %lndnm, !tbaa !1
  %lndnp = load i64, i64* %R1_Var
  %lndnq = add i64 %lndnp, 16
  %lndnr = inttoptr i64 %lndnq to i64*
  %lndns = load i64, i64* %lndnr, !tbaa !3
  store i64 %lndns, i64* %lsb8C
  %lndnv = load i64, i64* %R1_Var
  %lndnw = add i64 %lndnv, 24
  %lndnx = inttoptr i64 %lndnw to i64*
  %lndny = load i64, i64* %lndnx, !tbaa !3
  store i64 %lndny, i64* %lsb8D
  %lndnz = load i64, i64* %lsb8D
  %lndnA = icmp sge i64 %lndnz, 0
  %lndnB = zext i1 %lndnA to i64
  switch i64 %lndnB, label %cdkH [i64 0, label %cdkH
i64 1, label %cdkR]
cdkH:
  %lndnD = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdkF_info$def to i64
  %lndnC = load i64*, i64** %Sp_Var
  %lndnE = getelementptr inbounds i64, i64* %lndnC, i32 -4
  store i64 %lndnD, i64* %lndnE, !tbaa !1
  %lndnF = load i64, i64* %lsb8D
  store i64 %lndnF, i64* %R2_Var
  %lndnH = load i64, i64* %lsb8C
  %lndnG = load i64*, i64** %Sp_Var
  %lndnI = getelementptr inbounds i64, i64* %lndnG, i32 -3
  store i64 %lndnH, i64* %lndnI, !tbaa !1
  %lndnJ = load i64*, i64** %Sp_Var
  %lndnK = getelementptr inbounds i64, i64* %lndnJ, i32 -4
  %lndnL = ptrtoint i64* %lndnK to i64
  %lndnM = inttoptr i64 %lndnL to i64*
  store i64* %lndnM, i64** %Sp_Var
  %lndnN = bitcast i8* @integerzmgmp_GHCziIntegerziType_wordToInteger_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndnO = load i64*, i64** %Sp_Var
  %lndnP = load i64, i64* %R1_Var
  %lndnQ = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndnN( i64* %Base_Arg, i64* %lndnO, i64* %Hp_Arg, i64 %lndnP, i64 %lndnQ, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdkR:
  %lndnS = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdkQ_info$def to i64
  %lndnR = load i64*, i64** %Sp_Var
  %lndnT = getelementptr inbounds i64, i64* %lndnR, i32 -4
  store i64 %lndnS, i64* %lndnT, !tbaa !1
  %lndnU = load i64, i64* %lsb8D
  store i64 %lndnU, i64* %R2_Var
  %lndnW = load i64, i64* %lsb8C
  %lndnV = load i64*, i64** %Sp_Var
  %lndnX = getelementptr inbounds i64, i64* %lndnV, i32 -3
  store i64 %lndnW, i64* %lndnX, !tbaa !1
  %lndnY = load i64*, i64** %Sp_Var
  %lndnZ = getelementptr inbounds i64, i64* %lndnY, i32 -4
  %lndo0 = ptrtoint i64* %lndnZ to i64
  %lndo1 = inttoptr i64 %lndo0 to i64*
  store i64* %lndo1, i64** %Sp_Var
  %lndo2 = bitcast i8* @integerzmgmp_GHCziIntegerziType_smallInteger_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndo3 = load i64*, i64** %Sp_Var
  %lndo4 = load i64, i64* %R1_Var
  %lndo5 = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndo2( i64* %Base_Arg, i64* %lndo3, i64* %Hp_Arg, i64 %lndo4, i64 %lndo5, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdkv:
  %lndo6 = load i64, i64* %R1_Var
  store i64 %lndo6, i64* %R1_Var
  %lndo7 = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lndo8 = bitcast i64* %lndo7 to i64*
  %lndo9 = load i64, i64* %lndo8, !tbaa !4
  %lndoa = inttoptr i64 %lndo9 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndob = load i64*, i64** %Sp_Var
  %lndoc = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndoa( i64* %Base_Arg, i64* %lndob, i64* %Hp_Arg, i64 %lndoc, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdkQ_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdkQ_info$def to i8*)
define internal ghccc void @cdkQ_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdkQ_info$def to i64)),i64 240), i64 1, i64 4294967328}>
{
cdkQ:
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %lndod = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdkV_info$def to i64
  %lndoe = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lndod, i64* %lndoe, !tbaa !1
  %lndof = ptrtoint i8* @ghczmprim_GHCziTypes_ZMZN_closure to i64
  %lndog = add i64 %lndof, 1
  store i64 %lndog, i64* %R4_Var
  store i64 %R1_Arg, i64* %R3_Var
  store i64 0, i64* %R2_Var
  %lndoh = bitcast i8* @base_GHCziShow_zdwzdcshowsPrec3_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndoi = load i64, i64* %R2_Var
  %lndoj = load i64, i64* %R3_Var
  %lndok = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndoh( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lndoi, i64 %lndoj, i64 %lndok, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdkV_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdkV_info$def to i8*)
define internal ghccc void @cdkV_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdkV_info$def to i64)),i64 240), i64 1, i64 4294967328}>
{
cdkV:
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lndol = load i64*, i64** %Sp_Var
  %lndom = getelementptr inbounds i64, i64* %lndol, i32 -1
  store i64 %R2_Arg, i64* %lndom, !tbaa !1
  %lndon = load i64*, i64** %Sp_Var
  %lndoo = getelementptr inbounds i64, i64* %lndon, i32 0
  store i64 %R1_Arg, i64* %lndoo, !tbaa !1
  %lndop = load i64*, i64** %Sp_Var
  %lndoq = getelementptr inbounds i64, i64* %lndop, i32 -1
  %lndor = ptrtoint i64* %lndoq to i64
  %lndos = inttoptr i64 %lndor to i64*
  store i64* %lndos, i64** %Sp_Var
  %lndot = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdk5_entry$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndou = load i64*, i64** %Sp_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndot( i64* %Base_Arg, i64* %lndou, i64* %Hp_Arg, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdkF_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdkF_info$def to i8*)
define internal ghccc void @cdkF_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdkF_info$def to i64)),i64 240), i64 1, i64 4294967328}>
{
cdkF:
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %lndov = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdkL_info$def to i64
  %lndow = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lndov, i64* %lndow, !tbaa !1
  %lndox = ptrtoint i8* @ghczmprim_GHCziTypes_ZMZN_closure to i64
  %lndoy = add i64 %lndox, 1
  store i64 %lndoy, i64* %R4_Var
  store i64 %R1_Arg, i64* %R3_Var
  store i64 0, i64* %R2_Var
  %lndoz = bitcast i8* @base_GHCziShow_zdwzdcshowsPrec3_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndoA = load i64, i64* %R2_Var
  %lndoB = load i64, i64* %R3_Var
  %lndoC = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndoz( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lndoA, i64 %lndoB, i64 %lndoC, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdkL_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdkL_info$def to i8*)
define internal ghccc void @cdkL_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdkL_info$def to i64)),i64 240), i64 1, i64 4294967328}>
{
cdkL:
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lndoD = load i64*, i64** %Sp_Var
  %lndoE = getelementptr inbounds i64, i64* %lndoD, i32 0
  store i64 %R1_Arg, i64* %lndoE, !tbaa !1
  %lndoF = load i64*, i64** %Sp_Var
  %lndoG = getelementptr inbounds i64, i64* %lndoF, i32 -1
  store i64 %R2_Arg, i64* %lndoG, !tbaa !1
  %lndoH = load i64*, i64** %Sp_Var
  %lndoI = getelementptr inbounds i64, i64* %lndoH, i32 -1
  %lndoJ = ptrtoint i64* %lndoI to i64
  %lndoK = inttoptr i64 %lndoJ to i64*
  store i64* %lndoK, i64** %Sp_Var
  %lndoL = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdk5_entry$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndoM = load i64*, i64** %Sp_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndoL( i64* %Base_Arg, i64* %lndoM, i64* %Hp_Arg, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdk5_entry = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdk5_entry$def to i8*)
define internal ghccc void @cdk5_entry$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind
{
cdk5:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb8G = alloca i64, i32 1
  %lsb8H = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lndoN = load i64*, i64** %Hp_Var
  %lndoO = getelementptr inbounds i64, i64* %lndoN, i32 6
  %lndoP = ptrtoint i64* %lndoO to i64
  %lndoQ = inttoptr i64 %lndoP to i64*
  store i64* %lndoQ, i64** %Hp_Var
  %lndoR = load i64*, i64** %Sp_Var
  %lndoS = getelementptr inbounds i64, i64* %lndoR, i32 1
  %lndoT = bitcast i64* %lndoS to i64*
  %lndoU = load i64, i64* %lndoT, !tbaa !1
  store i64 %lndoU, i64* %lsb8G
  %lndoV = load i64*, i64** %Sp_Var
  %lndoW = getelementptr inbounds i64, i64* %lndoV, i32 0
  %lndoX = bitcast i64* %lndoW to i64*
  %lndoY = load i64, i64* %lndoX, !tbaa !1
  store i64 %lndoY, i64* %lsb8H
  %lndoZ = load i64*, i64** %Hp_Var
  %lndp0 = ptrtoint i64* %lndoZ to i64
  %lndp1 = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lndp2 = bitcast i64* %lndp1 to i64*
  %lndp3 = load i64, i64* %lndp2, !tbaa !4
  %lndp4 = icmp ugt i64 %lndp0, %lndp3
  br i1 %lndp4, label %cdkz, label %cdky
cdky:
  %lndp6 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb8N_info$def to i64
  %lndp5 = load i64*, i64** %Hp_Var
  %lndp7 = getelementptr inbounds i64, i64* %lndp5, i32 -5
  store i64 %lndp6, i64* %lndp7, !tbaa !2
  %lndp9 = load i64*, i64** %Sp_Var
  %lndpa = getelementptr inbounds i64, i64* %lndp9, i32 2
  %lndpb = bitcast i64* %lndpa to i64*
  %lndpc = load i64, i64* %lndpb, !tbaa !1
  %lndp8 = load i64*, i64** %Hp_Var
  %lndpd = getelementptr inbounds i64, i64* %lndp8, i32 -3
  store i64 %lndpc, i64* %lndpd, !tbaa !2
  %lndpf = ptrtoint i8* @ghczmprim_GHCziTypes_ZC_con_info to i64
  %lndpe = load i64*, i64** %Hp_Var
  %lndpg = getelementptr inbounds i64, i64* %lndpe, i32 -2
  store i64 %lndpf, i64* %lndpg, !tbaa !2
  %lndpi = load i64, i64* %lsb8G
  %lndph = load i64*, i64** %Hp_Var
  %lndpj = getelementptr inbounds i64, i64* %lndph, i32 -1
  store i64 %lndpi, i64* %lndpj, !tbaa !2
  %lndpl = load i64, i64* %lsb8H
  %lndpk = load i64*, i64** %Hp_Var
  %lndpm = getelementptr inbounds i64, i64* %lndpk, i32 0
  store i64 %lndpl, i64* %lndpm, !tbaa !2
  %lndpn = load i64*, i64** %Hp_Var
  %lndpo = getelementptr inbounds i64, i64* %lndpn, i32 -5
  %lndpp = ptrtoint i64* %lndpo to i64
  store i64 %lndpp, i64* %R3_Var
  %lndpr = load i64*, i64** %Hp_Var
  %lndps = ptrtoint i64* %lndpr to i64
  %lndpt = add i64 %lndps, -14
  store i64 %lndpt, i64* %R2_Var
  %lndpu = load i64*, i64** %Sp_Var
  %lndpv = getelementptr inbounds i64, i64* %lndpu, i32 3
  %lndpw = ptrtoint i64* %lndpv to i64
  %lndpx = inttoptr i64 %lndpw to i64*
  store i64* %lndpx, i64** %Sp_Var
  %lndpy = bitcast i8* @base_GHCziBase_zpzp_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndpz = load i64*, i64** %Sp_Var
  %lndpA = load i64*, i64** %Hp_Var
  %lndpB = load i64, i64* %R1_Var
  %lndpC = load i64, i64* %R2_Var
  %lndpD = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndpy( i64* %Base_Arg, i64* %lndpz, i64* %lndpA, i64 %lndpB, i64 %lndpC, i64 %lndpD, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdkz:
  %lndpE = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 48, i64* %lndpE, !tbaa !4
  %lndpG = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdk4_info$def to i64
  %lndpF = load i64*, i64** %Sp_Var
  %lndpH = getelementptr inbounds i64, i64* %lndpF, i32 1
  store i64 %lndpG, i64* %lndpH, !tbaa !1
  %lndpI = load i64, i64* %lsb8H
  store i64 %lndpI, i64* %R2_Var
  %lndpJ = load i64, i64* %lsb8G
  store i64 %lndpJ, i64* %R1_Var
  %lndpK = load i64*, i64** %Sp_Var
  %lndpL = getelementptr inbounds i64, i64* %lndpK, i32 1
  %lndpM = ptrtoint i64* %lndpL to i64
  %lndpN = inttoptr i64 %lndpM to i64*
  store i64* %lndpN, i64** %Sp_Var
  %lndpO = bitcast i8* @stg_gc_pp to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndpP = load i64*, i64** %Sp_Var
  %lndpQ = load i64*, i64** %Hp_Var
  %lndpR = load i64, i64* %R1_Var
  %lndpS = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndpO( i64* %Base_Arg, i64* %lndpP, i64* %lndpQ, i64 %lndpR, i64 %lndpS, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdk4_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdk4_info$def to i8*)
define internal ghccc void @cdk4_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdk4_info$def to i64)),i64 240), i64 1, i64 4294967328}>
{
cdk4:
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lndpT = load i64*, i64** %Sp_Var
  %lndpU = getelementptr inbounds i64, i64* %lndpT, i32 0
  store i64 %R1_Arg, i64* %lndpU, !tbaa !1
  %lndpV = load i64*, i64** %Sp_Var
  %lndpW = getelementptr inbounds i64, i64* %lndpV, i32 -1
  store i64 %R2_Arg, i64* %lndpW, !tbaa !1
  %lndpX = load i64*, i64** %Sp_Var
  %lndpY = getelementptr inbounds i64, i64* %lndpX, i32 -1
  %lndpZ = ptrtoint i64* %lndpY to i64
  %lndq0 = inttoptr i64 %lndpZ to i64*
  store i64* %lndq0, i64** %Sp_Var
  %lndq1 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdk5_entry$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndq2 = load i64*, i64** %Sp_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndq1( i64* %Base_Arg, i64* %lndq2, i64* %Hp_Arg, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@sb8Z_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb8Z_info$def to i8*)
define internal ghccc void @sb8Z_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb8Z_info$def to i64)),i64 240), i64 4294967297, i64 12884901908}>
{
cdl7:
  %lsb8Z = alloca i64, i32 1
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb8C = alloca i64, i32 1
  %lsb8D = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lndq3 = load i64, i64* %R1_Var
  store i64 %lndq3, i64* %lsb8Z
  %lndq4 = load i64*, i64** %Sp_Var
  %lndq5 = getelementptr inbounds i64, i64* %lndq4, i32 -2
  %lndq6 = ptrtoint i64* %lndq5 to i64
  %lndq7 = icmp ult i64 %lndq6, %SpLim_Arg
  br i1 %lndq7, label %cdl8, label %cdl9
cdl9:
  %lndq8 = load i64*, i64** %Hp_Var
  %lndq9 = getelementptr inbounds i64, i64* %lndq8, i32 4
  %lndqa = ptrtoint i64* %lndq9 to i64
  %lndqb = inttoptr i64 %lndqa to i64*
  store i64* %lndqb, i64** %Hp_Var
  %lndqc = load i64*, i64** %Hp_Var
  %lndqd = ptrtoint i64* %lndqc to i64
  %lndqe = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lndqf = bitcast i64* %lndqe to i64*
  %lndqg = load i64, i64* %lndqf, !tbaa !4
  %lndqh = icmp ugt i64 %lndqd, %lndqg
  br i1 %lndqh, label %cdlb, label %cdla
cdla:
  %lndqj = ptrtoint i8* @stg_upd_frame_info to i64
  %lndqi = load i64*, i64** %Sp_Var
  %lndqk = getelementptr inbounds i64, i64* %lndqi, i32 -2
  store i64 %lndqj, i64* %lndqk, !tbaa !1
  %lndqm = load i64, i64* %lsb8Z
  %lndql = load i64*, i64** %Sp_Var
  %lndqn = getelementptr inbounds i64, i64* %lndql, i32 -1
  store i64 %lndqm, i64* %lndqn, !tbaa !1
  %lndqo = load i64, i64* %lsb8Z
  %lndqp = add i64 %lndqo, 16
  %lndqq = inttoptr i64 %lndqp to i64*
  %lndqr = load i64, i64* %lndqq, !tbaa !5
  store i64 %lndqr, i64* %lsb8C
  %lndqs = load i64, i64* %lsb8Z
  %lndqt = add i64 %lndqs, 24
  %lndqu = inttoptr i64 %lndqt to i64*
  %lndqv = load i64, i64* %lndqu, !tbaa !5
  store i64 %lndqv, i64* %lsb8D
  %lndqx = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb8Y_info$def to i64
  %lndqw = load i64*, i64** %Hp_Var
  %lndqy = getelementptr inbounds i64, i64* %lndqw, i32 -3
  store i64 %lndqx, i64* %lndqy, !tbaa !2
  %lndqA = load i64, i64* %lsb8C
  %lndqz = load i64*, i64** %Hp_Var
  %lndqB = getelementptr inbounds i64, i64* %lndqz, i32 -1
  store i64 %lndqA, i64* %lndqB, !tbaa !2
  %lndqD = load i64, i64* %lsb8D
  %lndqC = load i64*, i64** %Hp_Var
  %lndqE = getelementptr inbounds i64, i64* %lndqC, i32 0
  store i64 %lndqD, i64* %lndqE, !tbaa !2
  %lndqF = load i64*, i64** %Hp_Var
  %lndqG = getelementptr inbounds i64, i64* %lndqF, i32 -3
  %lndqH = ptrtoint i64* %lndqG to i64
  store i64 %lndqH, i64* %R3_Var
  %lndqI = ptrtoint %cdl6_str_struct* @cdl6_str$def to i64
  store i64 %lndqI, i64* %R2_Var
  %lndqJ = load i64*, i64** %Sp_Var
  %lndqK = getelementptr inbounds i64, i64* %lndqJ, i32 -2
  %lndqL = ptrtoint i64* %lndqK to i64
  %lndqM = inttoptr i64 %lndqL to i64*
  store i64* %lndqM, i64** %Sp_Var
  %lndqN = bitcast i8* @ghczmprim_GHCziCString_unpackAppendCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndqO = load i64*, i64** %Sp_Var
  %lndqP = load i64*, i64** %Hp_Var
  %lndqQ = load i64, i64* %R1_Var
  %lndqR = load i64, i64* %R2_Var
  %lndqS = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndqN( i64* %Base_Arg, i64* %lndqO, i64* %lndqP, i64 %lndqQ, i64 %lndqR, i64 %lndqS, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdlb:
  %lndqT = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 32, i64* %lndqT, !tbaa !4
  br label %cdl8
cdl8:
  %lndqU = load i64, i64* %lsb8Z
  store i64 %lndqU, i64* %R1_Var
  %lndqV = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lndqW = bitcast i64* %lndqV to i64*
  %lndqX = load i64, i64* %lndqW, !tbaa !4
  %lndqY = inttoptr i64 %lndqX to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndqZ = load i64*, i64** %Sp_Var
  %lndr0 = load i64*, i64** %Hp_Var
  %lndr1 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndqY( i64* %Base_Arg, i64* %lndqZ, i64* %lndr0, i64 %lndr1, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@rb0S_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rb0S_info$def to i8*)
define internal ghccc void @rb0S_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rb0S_info$def to i64)),i64 240), i64 8589934606, i64 0, i64 246303489523727}>
{
cdlc:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lndr2 = load i64*, i64** %Hp_Var
  %lndr3 = getelementptr inbounds i64, i64* %lndr2, i32 4
  %lndr4 = ptrtoint i64* %lndr3 to i64
  %lndr5 = inttoptr i64 %lndr4 to i64*
  store i64* %lndr5, i64** %Hp_Var
  %lndr6 = load i64*, i64** %Hp_Var
  %lndr7 = ptrtoint i64* %lndr6 to i64
  %lndr8 = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lndr9 = bitcast i64* %lndr8 to i64*
  %lndra = load i64, i64* %lndr9, !tbaa !4
  %lndrb = icmp ugt i64 %lndr7, %lndra
  br i1 %lndrb, label %cdlg, label %cdlf
cdlf:
  %lndrd = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb8Z_info$def to i64
  %lndrc = load i64*, i64** %Hp_Var
  %lndre = getelementptr inbounds i64, i64* %lndrc, i32 -3
  store i64 %lndrd, i64* %lndre, !tbaa !2
  %lndrg = load i64, i64* %R2_Var
  %lndrf = load i64*, i64** %Hp_Var
  %lndrh = getelementptr inbounds i64, i64* %lndrf, i32 -1
  store i64 %lndrg, i64* %lndrh, !tbaa !2
  %lndrj = load i64, i64* %R3_Var
  %lndri = load i64*, i64** %Hp_Var
  %lndrk = getelementptr inbounds i64, i64* %lndri, i32 0
  store i64 %lndrj, i64* %lndrk, !tbaa !2
  %lndrl = load i64*, i64** %Hp_Var
  %lndrm = getelementptr inbounds i64, i64* %lndrl, i32 -3
  %lndrn = ptrtoint i64* %lndrm to i64
  store i64 %lndrn, i64* %R3_Var
  %lndro = ptrtoint %rb0A_closure_struct* @rb0A_closure$def to i64
  %lndrp = add i64 %lndro, 2
  store i64 %lndrp, i64* %R2_Var
  %lndrq = bitcast i8* @base_GHCziErr_error_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndrr = load i64*, i64** %Hp_Var
  %lndrs = load i64, i64* %R1_Var
  %lndrt = load i64, i64* %R2_Var
  %lndru = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndrq( i64* %Base_Arg, i64* %Sp_Arg, i64* %lndrr, i64 %lndrs, i64 %lndrt, i64 %lndru, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdlg:
  %lndrv = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 32, i64* %lndrv, !tbaa !4
  %lndrw = load i64, i64* %R3_Var
  store i64 %lndrw, i64* %R3_Var
  %lndrx = load i64, i64* %R2_Var
  store i64 %lndrx, i64* %R2_Var
  %lndry = ptrtoint %rb0S_closure_struct* @rb0S_closure$def to i64
  store i64 %lndry, i64* %R1_Var
  %lndrz = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lndrA = bitcast i64* %lndrz to i64*
  %lndrB = load i64, i64* %lndrA, !tbaa !4
  %lndrC = inttoptr i64 %lndrB to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndrD = load i64*, i64** %Hp_Var
  %lndrE = load i64, i64* %R1_Var
  %lndrF = load i64, i64* %R2_Var
  %lndrG = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndrC( i64* %Base_Arg, i64* %Sp_Arg, i64* %lndrD, i64 %lndrE, i64 %lndrF, i64 %lndrG, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_writezudisk1_closure_struct = type <{i64, i64}>
@Disk_writezudisk1_closure$def = internal global %Disk_writezudisk1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_writezudisk1_info$def to i64), i64 0}>
@Disk_writezudisk1_closure = alias i8* bitcast (%Disk_writezudisk1_closure_struct* @Disk_writezudisk1_closure$def to i8*)
@sb9P_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb9P_info$def to i8*)
define internal ghccc void @sb9P_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 4294967299, i64 12884901888, i64 9}>
{
cdtY:
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lcdu4 = alloca i64, i32 1
  %lcdu5 = alloca i64, i32 1
  %ludui = alloca i64, i32 1
  %luduj = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %luduf = alloca i64, i32 1
  %lsb9N = alloca i64, i32 1
  %ludug = alloca i64, i32 1
  %Base_Var = alloca i64*, i32 1
  store i64* %Base_Arg, i64** %Base_Var
  %luduh = alloca i64, i32 1
  %SpLim_Var = alloca i64, i32 1
  store i64 %SpLim_Arg, i64* %SpLim_Var
  %luduk = alloca i64, i32 1
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %ludul = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lndwg = load i64*, i64** %Sp_Var
  %lndwh = getelementptr inbounds i64, i64* %lndwg, i32 -1
  %lndwi = ptrtoint i64* %lndwh to i64
  %lndwj = load i64, i64* %SpLim_Var
  %lndwk = icmp ult i64 %lndwi, %lndwj
  br i1 %lndwk, label %cdua, label %cdub
cdub:
  %lndwm = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdu7_info$def to i64
  %lndwl = load i64*, i64** %Sp_Var
  %lndwn = getelementptr inbounds i64, i64* %lndwl, i32 -1
  store i64 %lndwm, i64* %lndwn, !tbaa !1
  %lndwo = load i64*, i64** %Sp_Var
  %lndwp = getelementptr inbounds i64, i64* %lndwo, i32 -1
  %lndwq = ptrtoint i64* %lndwp to i64
  %lndwr = inttoptr i64 %lndwq to i64*
  store i64* %lndwr, i64** %Sp_Var
  %lndwu = load i64, i64* %R1_Var
  %lndwv = add i64 %lndwu, 23
  %lndww = inttoptr i64 %lndwv to i64*
  %lndwx = load i64, i64* %lndww, !tbaa !3
  %lndwy = trunc i64 %lndwx to i32
  %lndwz = sext i32 %lndwy to i64
  store i64 %lndwz, i64* %lcdu4
  %lndwC = load i64, i64* %R1_Var
  %lndwD = add i64 %lndwC, 7
  %lndwE = inttoptr i64 %lndwD to i64*
  %lndwF = load i64, i64* %lndwE, !tbaa !3
  %lndwI = load i64, i64* %R1_Var
  %lndwJ = add i64 %lndwI, 15
  %lndwK = inttoptr i64 %lndwJ to i64*
  %lndwL = load i64, i64* %lndwK, !tbaa !3
  %lndwM = add i64 %lndwF, %lndwL
  store i64 %lndwM, i64* %lcdu5
  %lndwN = load i64*, i64** %Base_Var
  %lndwO = getelementptr inbounds i64, i64* %lndwN, i32 109
  %lndwP = bitcast i64* %lndwO to i64*
  %lndwQ = load i64, i64* %lndwP, !tbaa !4
  store i64 %lndwQ, i64* %ludui
  %lndwR = load i64, i64* %ludui
  %lndwS = add i64 %lndwR, 24
  %lndwT = inttoptr i64 %lndwS to i64*
  %lndwU = load i64, i64* %lndwT, !tbaa !5
  %lndwV = add i64 %lndwU, 16
  %lndwW = load i64*, i64** %Sp_Var
  %lndwX = ptrtoint i64* %lndwW to i64
  %lndwY = inttoptr i64 %lndwV to i64*
  store i64 %lndwX, i64* %lndwY, !tbaa !5
  %lndwZ = load i64*, i64** %Base_Var
  %lndx0 = getelementptr inbounds i64, i64* %lndwZ, i32 111
  %lndx1 = bitcast i64* %lndx0 to i64*
  %lndx2 = load i64, i64* %lndx1, !tbaa !4
  store i64 %lndx2, i64* %luduj
  %lndx3 = load i64, i64* %luduj
  %lndx4 = add i64 %lndx3, 8
  %lndx5 = load i64*, i64** %Hp_Var
  %lndx6 = getelementptr inbounds i64, i64* %lndx5, i32 1
  %lndx7 = ptrtoint i64* %lndx6 to i64
  %lndx8 = inttoptr i64 %lndx4 to i64*
  store i64 %lndx7, i64* %lndx8, !tbaa !5
  %lndx9 = load i64, i64* %ludui
  %lndxa = add i64 %lndx9, 104
  %lndxb = load i64, i64* %ludui
  %lndxc = add i64 %lndxb, 104
  %lndxd = inttoptr i64 %lndxc to i64*
  %lndxe = load i64, i64* %lndxd, !tbaa !5
  %lndxf = load i64*, i64** %Hp_Var
  %lndxg = getelementptr inbounds i64, i64* %lndxf, i32 1
  %lndxh = ptrtoint i64* %lndxg to i64
  %lndxi = load i64, i64* %luduj
  %lndxj = inttoptr i64 %lndxi to i64*
  %lndxk = load i64, i64* %lndxj, !tbaa !5
  %lndxl = sub i64 %lndxh, %lndxk
  %lndxm = sub i64 %lndxe, %lndxl
  %lndxn = inttoptr i64 %lndxa to i64*
  store i64 %lndxm, i64* %lndxn, !tbaa !5
  %lndxo = load i64*, i64** %Base_Var
  %lndxp = ptrtoint i64* %lndxo to i64
  %lndxq = inttoptr i64 %lndxp to i8*
  %lndxr = bitcast i8* @suspendThread to i8* (i8*, i64)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lndxs = call ccc i8* (i8*, i64) %lndxr( i8* %lndxq, i64 0 ) nounwind
  %lndxt = ptrtoint i8* %lndxs to i64
  store i64 %lndxt, i64* %luduf
  %lndxu = load i64, i64* %lcdu4
  %lndxv = load i64, i64* %lcdu5
  %lndxw = inttoptr i64 %lndxv to i8*
  %lndxx = bitcast i8* @write to i64 (i64, i8*, i64)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lndxy = call ccc i64 (i64, i8*, i64) %lndxx( i64 %lndxu, i8* %lndxw, i64 4096 ) nounwind
  store i64 %lndxy, i64* %lsb9N
  %lndxz = load i64, i64* %luduf
  %lndxA = inttoptr i64 %lndxz to i8*
  %lndxB = bitcast i8* @resumeThread to i8* (i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lndxC = call ccc i8* (i8*) %lndxB( i8* %lndxA ) nounwind
  %lndxD = ptrtoint i8* %lndxC to i64
  store i64 %lndxD, i64* %ludug
  %lndxE = load i64, i64* %ludug
  %lndxF = inttoptr i64 %lndxE to i64*
  store i64* %lndxF, i64** %Base_Var
  %lndxG = load i64*, i64** %Base_Var
  %lndxH = getelementptr inbounds i64, i64* %lndxG, i32 109
  %lndxI = bitcast i64* %lndxH to i64*
  %lndxJ = load i64, i64* %lndxI, !tbaa !4
  store i64 %lndxJ, i64* %ludui
  %lndxK = load i64, i64* %ludui
  %lndxL = add i64 %lndxK, 24
  %lndxM = inttoptr i64 %lndxL to i64*
  %lndxN = load i64, i64* %lndxM, !tbaa !5
  store i64 %lndxN, i64* %luduh
  %lndxO = load i64, i64* %luduh
  %lndxP = add i64 %lndxO, 16
  %lndxQ = inttoptr i64 %lndxP to i64*
  %lndxR = load i64, i64* %lndxQ, !tbaa !5
  %lndxS = inttoptr i64 %lndxR to i64*
  store i64* %lndxS, i64** %Sp_Var
  %lndxT = load i64, i64* %luduh
  %lndxU = add i64 %lndxT, 192
  store i64 %lndxU, i64* %SpLim_Var
  %lndxV = load i64*, i64** %Base_Var
  %lndxW = getelementptr inbounds i64, i64* %lndxV, i32 113
  store i64 0, i64* %lndxW, !tbaa !4
  %lndxX = load i64*, i64** %Base_Var
  %lndxY = getelementptr inbounds i64, i64* %lndxX, i32 111
  %lndxZ = bitcast i64* %lndxY to i64*
  %lndy0 = load i64, i64* %lndxZ, !tbaa !4
  store i64 %lndy0, i64* %luduj
  %lndy1 = load i64, i64* %luduj
  %lndy2 = add i64 %lndy1, 8
  %lndy3 = inttoptr i64 %lndy2 to i64*
  %lndy4 = load i64, i64* %lndy3, !tbaa !5
  store i64 %lndy4, i64* %luduk
  %lndy5 = load i64, i64* %luduk
  %lndy6 = add i64 %lndy5, -8
  %lndy7 = inttoptr i64 %lndy6 to i64*
  store i64* %lndy7, i64** %Hp_Var
  %lndy8 = load i64, i64* %luduj
  %lndy9 = inttoptr i64 %lndy8 to i64*
  %lndya = load i64, i64* %lndy9, !tbaa !5
  store i64 %lndya, i64* %ludul
  %lndyc = load i64, i64* %ludul
  %lndyd = load i64, i64* %luduj
  %lndye = add i64 %lndyd, 48
  %lndyf = inttoptr i64 %lndye to i32*
  %lndyg = load i32, i32* %lndyf, !tbaa !5
  %lndyh = sext i32 %lndyg to i64
  %lndyi = shl i64 %lndyh, 12
  %lndyj = add i64 %lndyi, -1
  %lndyk = add i64 %lndyc, %lndyj
  %lndyb = load i64*, i64** %Base_Var
  %lndyl = getelementptr inbounds i64, i64* %lndyb, i32 107
  store i64 %lndyk, i64* %lndyl, !tbaa !4
  %lndym = load i64, i64* %ludui
  %lndyn = add i64 %lndym, 104
  %lndyo = load i64, i64* %ludui
  %lndyp = add i64 %lndyo, 104
  %lndyq = inttoptr i64 %lndyp to i64*
  %lndyr = load i64, i64* %lndyq, !tbaa !5
  %lndys = load i64, i64* %luduk
  %lndyt = load i64, i64* %ludul
  %lndyu = sub i64 %lndys, %lndyt
  %lndyv = add i64 %lndyr, %lndyu
  %lndyw = inttoptr i64 %lndyn to i64*
  store i64 %lndyv, i64* %lndyw, !tbaa !5
  %lndyx = load i64, i64* %lsb9N
  store i64 %lndyx, i64* %R1_Var
  %lndyy = load i64*, i64** %Sp_Var
  %lndyz = getelementptr inbounds i64, i64* %lndyy, i32 0
  %lndyA = bitcast i64* %lndyz to i64*
  %lndyB = load i64, i64* %lndyA, !tbaa !1
  %lndyC = inttoptr i64 %lndyB to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndyD = load i64*, i64** %Base_Var
  %lndyE = load i64*, i64** %Sp_Var
  %lndyF = load i64*, i64** %Hp_Var
  %lndyG = load i64, i64* %R1_Var
  %lndyH = load i64, i64* %SpLim_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndyC( i64* %lndyD, i64* %lndyE, i64* %lndyF, i64 %lndyG, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %lndyH ) nounwind
  ret void
cdua:
  %lndyI = load i64, i64* %R1_Var
  store i64 %lndyI, i64* %R1_Var
  %lndyJ = load i64*, i64** %Base_Var
  %lndyK = getelementptr inbounds i64, i64* %lndyJ, i32 -1
  %lndyL = bitcast i64* %lndyK to i64*
  %lndyM = load i64, i64* %lndyL, !tbaa !4
  %lndyN = inttoptr i64 %lndyM to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndyO = load i64*, i64** %Base_Var
  %lndyP = load i64*, i64** %Sp_Var
  %lndyQ = load i64*, i64** %Hp_Var
  %lndyR = load i64, i64* %R1_Var
  %lndyS = load i64, i64* %SpLim_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndyN( i64* %lndyO, i64* %lndyP, i64* %lndyQ, i64 %lndyR, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %lndyS ) nounwind
  ret void
}
@cdu7_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdu7_info$def to i8*)
define internal ghccc void @cdu7_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
cdu7:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lndyT = load i64*, i64** %Hp_Var
  %lndyU = getelementptr inbounds i64, i64* %lndyT, i32 2
  %lndyV = ptrtoint i64* %lndyU to i64
  %lndyW = inttoptr i64 %lndyV to i64*
  store i64* %lndyW, i64** %Hp_Var
  %lndyX = load i64*, i64** %Hp_Var
  %lndyY = ptrtoint i64* %lndyX to i64
  %lndyZ = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lndz0 = bitcast i64* %lndyZ to i64*
  %lndz1 = load i64, i64* %lndz0, !tbaa !4
  %lndz2 = icmp ugt i64 %lndyY, %lndz1
  br i1 %lndz2, label %cdue, label %cdud
cdud:
  %lndz4 = ptrtoint i8* @base_GHCziInt_I64zh_con_info to i64
  %lndz3 = load i64*, i64** %Hp_Var
  %lndz5 = getelementptr inbounds i64, i64* %lndz3, i32 -1
  store i64 %lndz4, i64* %lndz5, !tbaa !2
  %lndz7 = load i64, i64* %R1_Var
  %lndz6 = load i64*, i64** %Hp_Var
  %lndz8 = getelementptr inbounds i64, i64* %lndz6, i32 0
  store i64 %lndz7, i64* %lndz8, !tbaa !2
  %lndza = load i64*, i64** %Hp_Var
  %lndzb = ptrtoint i64* %lndza to i64
  %lndzc = add i64 %lndzb, -7
  store i64 %lndzc, i64* %R1_Var
  %lndzd = load i64*, i64** %Sp_Var
  %lndze = getelementptr inbounds i64, i64* %lndzd, i32 1
  %lndzf = ptrtoint i64* %lndze to i64
  %lndzg = inttoptr i64 %lndzf to i64*
  store i64* %lndzg, i64** %Sp_Var
  %lndzh = load i64*, i64** %Sp_Var
  %lndzi = getelementptr inbounds i64, i64* %lndzh, i32 0
  %lndzj = bitcast i64* %lndzi to i64*
  %lndzk = load i64, i64* %lndzj, !tbaa !1
  %lndzl = inttoptr i64 %lndzk to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndzm = load i64*, i64** %Sp_Var
  %lndzn = load i64*, i64** %Hp_Var
  %lndzo = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndzl( i64* %Base_Arg, i64* %lndzm, i64* %lndzn, i64 %lndzo, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdue:
  %lndzp = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 16, i64* %lndzp, !tbaa !4
  %lndzq = load i64, i64* %R1_Var
  store i64 %lndzq, i64* %R1_Var
  %lndzr = bitcast i8* @stg_gc_unbx_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndzs = load i64*, i64** %Sp_Var
  %lndzt = load i64*, i64** %Hp_Var
  %lndzu = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndzr( i64* %Base_Arg, i64* %lndzs, i64* %lndzt, i64 %lndzu, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@Disk_writezudisk1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_writezudisk1_info$def to i8*)
define ghccc void @Disk_writezudisk1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_writezudisk1_info$def to i64)),i64 176), i64 17179869207, i64 0, i64 1117543622766493711}>
{
cdus:
  %R4_Var = alloca i64, i32 1
  store i64 %R4_Arg, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lndzv = load i64*, i64** %Sp_Var
  %lndzw = getelementptr inbounds i64, i64* %lndzv, i32 -9
  %lndzx = ptrtoint i64* %lndzw to i64
  %lndzy = icmp uge i64 %lndzx, %SpLim_Arg
  br i1 %lndzy, label %udvY, label %cdut
cdut:
  %lndzz = load i64, i64* %R4_Var
  store i64 %lndzz, i64* %R4_Var
  %lndzA = load i64, i64* %R3_Var
  store i64 %lndzA, i64* %R3_Var
  %lndzB = load i64, i64* %R2_Var
  store i64 %lndzB, i64* %R2_Var
  %lndzC = ptrtoint %Disk_writezudisk1_closure_struct* @Disk_writezudisk1_closure$def to i64
  store i64 %lndzC, i64* %R1_Var
  %lndzD = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lndzE = bitcast i64* %lndzD to i64*
  %lndzF = load i64, i64* %lndzE, !tbaa !4
  %lndzG = inttoptr i64 %lndzF to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndzH = load i64*, i64** %Sp_Var
  %lndzI = load i64, i64* %R1_Var
  %lndzJ = load i64, i64* %R2_Var
  %lndzK = load i64, i64* %R3_Var
  %lndzL = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndzG( i64* %Base_Arg, i64* %lndzH, i64* %Hp_Arg, i64 %lndzI, i64 %lndzJ, i64 %lndzK, i64 %lndzL, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
udvY:
  %lndzN = load i64, i64* %R2_Var
  %lndzM = load i64*, i64** %Sp_Var
  %lndzO = getelementptr inbounds i64, i64* %lndzM, i32 -3
  store i64 %lndzN, i64* %lndzO, !tbaa !1
  %lndzQ = load i64, i64* %R3_Var
  %lndzP = load i64*, i64** %Sp_Var
  %lndzR = getelementptr inbounds i64, i64* %lndzP, i32 -2
  store i64 %lndzQ, i64* %lndzR, !tbaa !1
  %lndzT = load i64, i64* %R4_Var
  %lndzS = load i64*, i64** %Sp_Var
  %lndzU = getelementptr inbounds i64, i64* %lndzS, i32 -1
  store i64 %lndzT, i64* %lndzU, !tbaa !1
  %lndzV = load i64*, i64** %Sp_Var
  %lndzW = getelementptr inbounds i64, i64* %lndzV, i32 -3
  %lndzX = ptrtoint i64* %lndzW to i64
  %lndzY = inttoptr i64 %lndzX to i64*
  store i64* %lndzY, i64** %Sp_Var
  %lndzZ = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdsX_entry$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndA0 = load i64*, i64** %Sp_Var
  %lndA1 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndzZ( i64* %Base_Arg, i64* %lndA0, i64* %Hp_Arg, i64 %lndA1, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdsX_entry = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdsX_entry$def to i8*)
define internal ghccc void @cdsX_entry$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind
{
cdsX:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lndA3 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdt0_info$def to i64
  %lndA2 = load i64*, i64** %Sp_Var
  %lndA4 = getelementptr inbounds i64, i64* %lndA2, i32 -1
  store i64 %lndA3, i64* %lndA4, !tbaa !1
  %lndA5 = load i64*, i64** %Sp_Var
  %lndA6 = getelementptr inbounds i64, i64* %lndA5, i32 2
  %lndA7 = bitcast i64* %lndA6 to i64*
  %lndA8 = load i64, i64* %lndA7, !tbaa !1
  store i64 %lndA8, i64* %R1_Var
  %lndA9 = load i64*, i64** %Sp_Var
  %lndAa = getelementptr inbounds i64, i64* %lndA9, i32 -1
  %lndAb = ptrtoint i64* %lndAa to i64
  %lndAc = inttoptr i64 %lndAb to i64*
  store i64* %lndAc, i64** %Sp_Var
  %lndAd = load i64, i64* %R1_Var
  %lndAe = and i64 %lndAd, 7
  %lndAf = icmp ne i64 %lndAe, 0
  br i1 %lndAf, label %udw2, label %cdt1
cdt1:
  %lndAh = load i64, i64* %R1_Var
  %lndAi = inttoptr i64 %lndAh to i64*
  %lndAj = load i64, i64* %lndAi, !tbaa !3
  %lndAk = inttoptr i64 %lndAj to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndAl = load i64*, i64** %Sp_Var
  %lndAm = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndAk( i64* %Base_Arg, i64* %lndAl, i64* %Hp_Arg, i64 %lndAm, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
udw2:
  %lndAn = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdt0_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndAo = load i64*, i64** %Sp_Var
  %lndAp = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndAn( i64* %Base_Arg, i64* %lndAo, i64* %Hp_Arg, i64 %lndAp, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdt0_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdt0_info$def to i8*)
define internal ghccc void @cdt0_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdt0_info$def to i64)),i64 176), i64 259, i64 973428434690637856}>
{
cdt0:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lsb94 = alloca i64, i32 1
  %lndAq = load i64, i64* %R1_Var
  %lndAr = and i64 %lndAq, 7
  switch i64 %lndAr, label %cduo [i64 1, label %cduo
i64 2, label %cdup
i64 3, label %cduq]
cduo:
  %lndAt = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cduw_info$def to i64
  %lndAs = load i64*, i64** %Sp_Var
  %lndAu = getelementptr inbounds i64, i64* %lndAs, i32 0
  store i64 %lndAt, i64* %lndAu, !tbaa !1
  store i64 4096, i64* %R3_Var
  %lndAx = load i64, i64* %R1_Var
  %lndAy = add i64 %lndAx, 7
  %lndAz = inttoptr i64 %lndAy to i64*
  %lndAA = load i64, i64* %lndAz, !tbaa !3
  store i64 %lndAA, i64* %R2_Var
  %lndAB = bitcast i8* @Word_zdwi2bs_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndAC = load i64*, i64** %Sp_Var
  %lndAD = load i64, i64* %R1_Var
  %lndAE = load i64, i64* %R2_Var
  %lndAF = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndAB( i64* %Base_Arg, i64* %lndAC, i64* %Hp_Arg, i64 %lndAD, i64 %lndAE, i64 %lndAF, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdup:
  %lndAG = ptrtoint %rb0R_closure_struct* @rb0R_closure$def to i64
  store i64 %lndAG, i64* %R1_Var
  %lndAH = load i64*, i64** %Sp_Var
  %lndAI = getelementptr inbounds i64, i64* %lndAH, i32 4
  %lndAJ = ptrtoint i64* %lndAI to i64
  %lndAK = inttoptr i64 %lndAJ to i64*
  store i64* %lndAK, i64** %Sp_Var
  %lndAL = bitcast i8* @stg_ap_0_fast to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndAM = load i64*, i64** %Sp_Var
  %lndAN = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndAL( i64* %Base_Arg, i64* %lndAM, i64* %Hp_Arg, i64 %lndAN, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cduq:
  %lndAP = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdt7_info$def to i64
  %lndAO = load i64*, i64** %Sp_Var
  %lndAQ = getelementptr inbounds i64, i64* %lndAO, i32 0
  store i64 %lndAP, i64* %lndAQ, !tbaa !1
  %lndAR = load i64, i64* %R1_Var
  store i64 %lndAR, i64* %lsb94
  %lndAU = load i64, i64* %R1_Var
  %lndAV = add i64 %lndAU, 5
  %lndAW = inttoptr i64 %lndAV to i64*
  %lndAX = load i64, i64* %lndAW, !tbaa !3
  store i64 %lndAX, i64* %R1_Var
  %lndAZ = load i64, i64* %lsb94
  %lndAY = load i64*, i64** %Sp_Var
  %lndB0 = getelementptr inbounds i64, i64* %lndAY, i32 3
  store i64 %lndAZ, i64* %lndB0, !tbaa !1
  %lndB1 = load i64, i64* %R1_Var
  %lndB2 = and i64 %lndB1, 7
  %lndB3 = icmp ne i64 %lndB2, 0
  br i1 %lndB3, label %udw3, label %cdt8
cdt8:
  %lndB5 = load i64, i64* %R1_Var
  %lndB6 = inttoptr i64 %lndB5 to i64*
  %lndB7 = load i64, i64* %lndB6, !tbaa !3
  %lndB8 = inttoptr i64 %lndB7 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndB9 = load i64*, i64** %Sp_Var
  %lndBa = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndB8( i64* %Base_Arg, i64* %lndB9, i64* %Hp_Arg, i64 %lndBa, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
udw3:
  %lndBb = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdt7_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndBc = load i64*, i64** %Sp_Var
  %lndBd = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndBb( i64* %Base_Arg, i64* %lndBc, i64* %Hp_Arg, i64 %lndBd, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdt7_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdt7_info$def to i8*)
define internal ghccc void @cdt7_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdt7_info$def to i64)),i64 176), i64 3, i64 324910088349286432}>
{
cdt7:
  %lsb9f = alloca i64, i32 1
  %lsb9e = alloca i64, i32 1
  %lsb9g = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lndBf = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdtc_info$def to i64
  %lndBe = load i64*, i64** %Sp_Var
  %lndBg = getelementptr inbounds i64, i64* %lndBe, i32 -2
  store i64 %lndBf, i64* %lndBg, !tbaa !1
  %lndBj = load i64, i64* %R1_Var
  %lndBk = add i64 %lndBj, 7
  %lndBl = inttoptr i64 %lndBk to i64*
  %lndBm = load i64, i64* %lndBl, !tbaa !3
  store i64 %lndBm, i64* %lsb9f
  %lndBp = load i64, i64* %R1_Var
  %lndBq = add i64 %lndBp, 15
  %lndBr = inttoptr i64 %lndBq to i64*
  %lndBs = load i64, i64* %lndBr, !tbaa !3
  store i64 %lndBs, i64* %lsb9e
  %lndBv = load i64, i64* %R1_Var
  %lndBw = add i64 %lndBv, 23
  %lndBx = inttoptr i64 %lndBw to i64*
  %lndBy = load i64, i64* %lndBx, !tbaa !3
  store i64 %lndBy, i64* %lsb9g
  %lndBz = load i64*, i64** %Sp_Var
  %lndBA = getelementptr inbounds i64, i64* %lndBz, i32 1
  %lndBB = bitcast i64* %lndBA to i64*
  %lndBC = load i64, i64* %lndBB, !tbaa !1
  store i64 %lndBC, i64* %R1_Var
  %lndBE = load i64, i64* %lsb9g
  %lndBD = load i64*, i64** %Sp_Var
  %lndBF = getelementptr inbounds i64, i64* %lndBD, i32 -1
  store i64 %lndBE, i64* %lndBF, !tbaa !1
  %lndBH = load i64, i64* %lsb9f
  %lndBG = load i64*, i64** %Sp_Var
  %lndBI = getelementptr inbounds i64, i64* %lndBG, i32 0
  store i64 %lndBH, i64* %lndBI, !tbaa !1
  %lndBK = load i64, i64* %lsb9e
  %lndBJ = load i64*, i64** %Sp_Var
  %lndBL = getelementptr inbounds i64, i64* %lndBJ, i32 1
  store i64 %lndBK, i64* %lndBL, !tbaa !1
  %lndBM = load i64*, i64** %Sp_Var
  %lndBN = getelementptr inbounds i64, i64* %lndBM, i32 -2
  %lndBO = ptrtoint i64* %lndBN to i64
  %lndBP = inttoptr i64 %lndBO to i64*
  store i64* %lndBP, i64** %Sp_Var
  %lndBQ = load i64, i64* %R1_Var
  %lndBR = and i64 %lndBQ, 7
  %lndBS = icmp ne i64 %lndBR, 0
  br i1 %lndBS, label %udw4, label %cdtd
cdtd:
  %lndBU = load i64, i64* %R1_Var
  %lndBV = inttoptr i64 %lndBU to i64*
  %lndBW = load i64, i64* %lndBV, !tbaa !3
  %lndBX = inttoptr i64 %lndBW to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndBY = load i64*, i64** %Sp_Var
  %lndBZ = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndBX( i64* %Base_Arg, i64* %lndBY, i64* %Hp_Arg, i64 %lndBZ, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
udw4:
  %lndC0 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdtc_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndC1 = load i64*, i64** %Sp_Var
  %lndC2 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndC0( i64* %Base_Arg, i64* %lndC1, i64* %Hp_Arg, i64 %lndC2, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdtc_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdtc_info$def to i8*)
define internal ghccc void @cdtc_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdtc_info$def to i64)),i64 176), i64 325, i64 324910088349286432}>
{
cdtc:
  %lsb9k = alloca i64, i32 1
  %lsb9l = alloca i64, i32 1
  %lsb9j = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lndC4 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdth_info$def to i64
  %lndC3 = load i64*, i64** %Sp_Var
  %lndC5 = getelementptr inbounds i64, i64* %lndC3, i32 -3
  store i64 %lndC4, i64* %lndC5, !tbaa !1
  %lndC8 = load i64, i64* %R1_Var
  %lndC9 = add i64 %lndC8, 7
  %lndCa = inttoptr i64 %lndC9 to i64*
  %lndCb = load i64, i64* %lndCa, !tbaa !3
  store i64 %lndCb, i64* %lsb9k
  %lndCe = load i64, i64* %R1_Var
  %lndCf = add i64 %lndCe, 15
  %lndCg = inttoptr i64 %lndCf to i64*
  %lndCh = load i64, i64* %lndCg, !tbaa !3
  store i64 %lndCh, i64* %lsb9l
  %lndCk = load i64, i64* %R1_Var
  %lndCl = add i64 %lndCk, 31
  %lndCm = inttoptr i64 %lndCl to i64*
  %lndCn = load i64, i64* %lndCm, !tbaa !3
  store i64 %lndCn, i64* %lsb9j
  %lndCo = load i64, i64* %lsb9k
  %lndCp = add i64 %lndCo, 8
  %lndCq = inttoptr i64 %lndCp to i64*
  %lndCr = load i64, i64* %lndCq, !tbaa !5
  store i64 %lndCr, i64* %R1_Var
  %lndCt = load i64, i64* %lsb9k
  %lndCs = load i64*, i64** %Sp_Var
  %lndCu = getelementptr inbounds i64, i64* %lndCs, i32 -2
  store i64 %lndCt, i64* %lndCu, !tbaa !1
  %lndCw = load i64, i64* %lsb9l
  %lndCv = load i64*, i64** %Sp_Var
  %lndCx = getelementptr inbounds i64, i64* %lndCv, i32 -1
  store i64 %lndCw, i64* %lndCx, !tbaa !1
  %lndCz = load i64, i64* %lsb9j
  %lndCy = load i64*, i64** %Sp_Var
  %lndCA = getelementptr inbounds i64, i64* %lndCy, i32 0
  store i64 %lndCz, i64* %lndCA, !tbaa !1
  %lndCB = load i64*, i64** %Sp_Var
  %lndCC = getelementptr inbounds i64, i64* %lndCB, i32 -3
  %lndCD = ptrtoint i64* %lndCC to i64
  %lndCE = inttoptr i64 %lndCD to i64*
  store i64* %lndCE, i64** %Sp_Var
  %lndCF = load i64, i64* %R1_Var
  %lndCG = and i64 %lndCF, 7
  %lndCH = icmp ne i64 %lndCG, 0
  br i1 %lndCH, label %udw5, label %cdti
cdti:
  %lndCJ = load i64, i64* %R1_Var
  %lndCK = inttoptr i64 %lndCJ to i64*
  %lndCL = load i64, i64* %lndCK, !tbaa !3
  %lndCM = inttoptr i64 %lndCL to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndCN = load i64*, i64** %Sp_Var
  %lndCO = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndCM( i64* %Base_Arg, i64* %lndCN, i64* %Hp_Arg, i64 %lndCO, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
udw5:
  %lndCP = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdth_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndCQ = load i64*, i64** %Sp_Var
  %lndCR = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndCP( i64* %Base_Arg, i64* %lndCQ, i64* %Hp_Arg, i64 %lndCR, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdth_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdth_info$def to i8*)
define internal ghccc void @cdth_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdth_info$def to i64)),i64 176), i64 2824, i64 324910088349286432}>
{
cdth:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsb9r = alloca i64, i32 1
  %lsb9t = alloca i64, i32 1
  %lsb9u = alloca i64, i32 1
  %lsb9k = alloca i64, i32 1
  %lsb9l = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lndCS = load i64*, i64** %Hp_Var
  %lndCT = getelementptr inbounds i64, i64* %lndCS, i32 4
  %lndCU = ptrtoint i64* %lndCT to i64
  %lndCV = inttoptr i64 %lndCU to i64*
  store i64* %lndCV, i64** %Hp_Var
  %lndCW = load i64*, i64** %Hp_Var
  %lndCX = ptrtoint i64* %lndCW to i64
  %lndCY = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lndCZ = bitcast i64* %lndCY to i64*
  %lndD0 = load i64, i64* %lndCZ, !tbaa !4
  %lndD1 = icmp ugt i64 %lndCX, %lndD0
  br i1 %lndD1, label %cduM, label %cduL
cduL:
  %lndD4 = load i64, i64* %R1_Var
  %lndD5 = add i64 %lndD4, 7
  %lndD6 = inttoptr i64 %lndD5 to i64*
  %lndD7 = load i64, i64* %lndD6, !tbaa !3
  store i64 %lndD7, i64* %lsb9r
  %lndDa = load i64, i64* %R1_Var
  %lndDb = add i64 %lndDa, 23
  %lndDc = inttoptr i64 %lndDb to i64*
  %lndDd = load i64, i64* %lndDc, !tbaa !3
  store i64 %lndDd, i64* %lsb9t
  %lndDg = load i64, i64* %R1_Var
  %lndDh = add i64 %lndDg, 15
  %lndDi = inttoptr i64 %lndDh to i64*
  %lndDj = load i64, i64* %lndDi, !tbaa !3
  %lndDk = add i64 %lndDj, 1
  store i64 %lndDk, i64* %lsb9u
  %lndDm = ptrtoint i8* @Disk_Stats_con_info to i64
  %lndDl = load i64*, i64** %Hp_Var
  %lndDn = getelementptr inbounds i64, i64* %lndDl, i32 -3
  store i64 %lndDm, i64* %lndDn, !tbaa !2
  %lndDp = load i64, i64* %lsb9r
  %lndDo = load i64*, i64** %Hp_Var
  %lndDq = getelementptr inbounds i64, i64* %lndDo, i32 -2
  store i64 %lndDp, i64* %lndDq, !tbaa !2
  %lndDs = load i64, i64* %lsb9u
  %lndDr = load i64*, i64** %Hp_Var
  %lndDt = getelementptr inbounds i64, i64* %lndDr, i32 -1
  store i64 %lndDs, i64* %lndDt, !tbaa !2
  %lndDv = load i64, i64* %lsb9t
  %lndDu = load i64*, i64** %Hp_Var
  %lndDw = getelementptr inbounds i64, i64* %lndDu, i32 0
  store i64 %lndDv, i64* %lndDw, !tbaa !2
  %lndDx = load i64*, i64** %Sp_Var
  %lndDy = getelementptr inbounds i64, i64* %lndDx, i32 1
  %lndDz = bitcast i64* %lndDy to i64*
  %lndDA = load i64, i64* %lndDz, !tbaa !1
  store i64 %lndDA, i64* %lsb9k
  %lndDB = load i64, i64* %lsb9k
  %lndDC = add i64 %lndDB, 8
  %lndDE = load i64*, i64** %Hp_Var
  %lndDF = ptrtoint i64* %lndDE to i64
  %lndDG = add i64 %lndDF, -23
  %lndDH = inttoptr i64 %lndDC to i64*
  store i64 %lndDG, i64* %lndDH, !tbaa !5
  %lndDI = load i64*, i64** %Sp_Var
  %lndDJ = getelementptr inbounds i64, i64* %lndDI, i32 2
  %lndDK = bitcast i64* %lndDJ to i64*
  %lndDL = load i64, i64* %lndDK, !tbaa !1
  store i64 %lndDL, i64* %lsb9l
  %lndDM = ptrtoint i64* %Base_Arg to i64
  %lndDN = inttoptr i64 %lndDM to i8*
  %lndDO = load i64, i64* %lsb9k
  %lndDP = inttoptr i64 %lndDO to i8*
  %lndDQ = bitcast i8* @dirty_MUT_VAR to void (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  call ccc void (i8*, i8*) %lndDQ( i8* %lndDN, i8* %lndDP ) nounwind
  %lndDS = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cduN_info$def to i64
  %lndDR = load i64*, i64** %Sp_Var
  %lndDT = getelementptr inbounds i64, i64* %lndDR, i32 2
  store i64 %lndDS, i64* %lndDT, !tbaa !1
  %lndDU = load i64, i64* %lsb9l
  store i64 %lndDU, i64* %R1_Var
  %lndDV = load i64*, i64** %Sp_Var
  %lndDW = getelementptr inbounds i64, i64* %lndDV, i32 2
  %lndDX = ptrtoint i64* %lndDW to i64
  %lndDY = inttoptr i64 %lndDX to i64*
  store i64* %lndDY, i64** %Sp_Var
  %lndDZ = load i64, i64* %R1_Var
  %lndE0 = and i64 %lndDZ, 7
  %lndE1 = icmp ne i64 %lndE0, 0
  br i1 %lndE1, label %udw6, label %cdvr
cdvr:
  %lndE3 = load i64, i64* %R1_Var
  %lndE4 = inttoptr i64 %lndE3 to i64*
  %lndE5 = load i64, i64* %lndE4, !tbaa !3
  %lndE6 = inttoptr i64 %lndE5 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndE7 = load i64*, i64** %Sp_Var
  %lndE8 = load i64*, i64** %Hp_Var
  %lndE9 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndE6( i64* %Base_Arg, i64* %lndE7, i64* %lndE8, i64 %lndE9, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
udw6:
  %lndEa = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cduN_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndEb = load i64*, i64** %Sp_Var
  %lndEc = load i64*, i64** %Hp_Var
  %lndEd = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndEa( i64* %Base_Arg, i64* %lndEb, i64* %lndEc, i64 %lndEd, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cduM:
  %lndEe = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 32, i64* %lndEe, !tbaa !4
  %lndEf = load i64, i64* %R1_Var
  store i64 %lndEf, i64* %R1_Var
  %lndEg = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndEh = load i64*, i64** %Sp_Var
  %lndEi = load i64*, i64** %Hp_Var
  %lndEj = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndEg( i64* %Base_Arg, i64* %lndEh, i64* %lndEi, i64 %lndEj, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cduN_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cduN_info$def to i8*)
define internal ghccc void @cduN_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cduN_info$def to i64)),i64 176), i64 710, i64 324910088349286432}>
{
cduN:
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lndEk = load i64, i64* %R1_Var
  %lndEl = and i64 %lndEk, 7
  switch i64 %lndEl, label %udvZ [i64 1, label %udvZ
i64 2, label %cdvH]
udvZ:
  %lndEm = load i64*, i64** %Sp_Var
  %lndEn = getelementptr inbounds i64, i64* %lndEm, i32 1
  %lndEo = ptrtoint i64* %lndEn to i64
  %lndEp = inttoptr i64 %lndEo to i64*
  store i64* %lndEp, i64** %Sp_Var
  %lndEq = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb9x_entry$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndEr = load i64*, i64** %Sp_Var
  %lndEs = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndEq( i64* %Base_Arg, i64* %lndEr, i64* %Hp_Arg, i64 %lndEs, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdvH:
  %lndEu = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdvA_info$def to i64
  %lndEt = load i64*, i64** %Sp_Var
  %lndEv = getelementptr inbounds i64, i64* %lndEt, i32 0
  store i64 %lndEu, i64* %lndEv, !tbaa !1
  %lndEy = load i64, i64* %R1_Var
  %lndEz = add i64 %lndEy, 6
  %lndEA = inttoptr i64 %lndEz to i64*
  %lndEB = load i64, i64* %lndEA, !tbaa !3
  store i64 %lndEB, i64* %R1_Var
  %lndEC = load i64, i64* %R1_Var
  %lndED = and i64 %lndEC, 7
  %lndEE = icmp ne i64 %lndED, 0
  br i1 %lndEE, label %udwa, label %cdvB
cdvB:
  %lndEG = load i64, i64* %R1_Var
  %lndEH = inttoptr i64 %lndEG to i64*
  %lndEI = load i64, i64* %lndEH, !tbaa !3
  %lndEJ = inttoptr i64 %lndEI to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndEK = load i64*, i64** %Sp_Var
  %lndEL = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndEJ( i64* %Base_Arg, i64* %lndEK, i64* %Hp_Arg, i64 %lndEL, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
udwa:
  %lndEM = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdvA_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndEN = load i64*, i64** %Sp_Var
  %lndEO = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndEM( i64* %Base_Arg, i64* %lndEN, i64* %Hp_Arg, i64 %lndEO, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdvA_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdvA_info$def to i8*)
define internal ghccc void @cdvA_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdvA_info$def to i64)),i64 176), i64 710, i64 324910088349286432}>
{
cdvA:
  %lsba5 = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lndEQ = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdvF_info$def to i64
  %lndEP = load i64*, i64** %Sp_Var
  %lndER = getelementptr inbounds i64, i64* %lndEP, i32 -1
  store i64 %lndEQ, i64* %lndER, !tbaa !1
  %lndEU = load i64, i64* %R1_Var
  %lndEV = add i64 %lndEU, 7
  %lndEW = inttoptr i64 %lndEV to i64*
  %lndEX = load i64, i64* %lndEW, !tbaa !3
  store i64 %lndEX, i64* %lsba5
  %lndEY = load i64, i64* %lsba5
  %lndEZ = add i64 %lndEY, 8
  %lndF0 = inttoptr i64 %lndEZ to i64*
  %lndF1 = load i64, i64* %lndF0, !tbaa !5
  store i64 %lndF1, i64* %R1_Var
  %lndF3 = load i64, i64* %lsba5
  %lndF2 = load i64*, i64** %Sp_Var
  %lndF4 = getelementptr inbounds i64, i64* %lndF2, i32 0
  store i64 %lndF3, i64* %lndF4, !tbaa !1
  %lndF5 = load i64*, i64** %Sp_Var
  %lndF6 = getelementptr inbounds i64, i64* %lndF5, i32 -1
  %lndF7 = ptrtoint i64* %lndF6 to i64
  %lndF8 = inttoptr i64 %lndF7 to i64*
  store i64* %lndF8, i64** %Sp_Var
  %lndF9 = load i64, i64* %R1_Var
  %lndFa = and i64 %lndF9, 7
  %lndFb = icmp ne i64 %lndFa, 0
  br i1 %lndFb, label %udwe, label %cdvJ
cdvJ:
  %lndFd = load i64, i64* %R1_Var
  %lndFe = inttoptr i64 %lndFd to i64*
  %lndFf = load i64, i64* %lndFe, !tbaa !3
  %lndFg = inttoptr i64 %lndFf to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndFh = load i64*, i64** %Sp_Var
  %lndFi = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndFg( i64* %Base_Arg, i64* %lndFh, i64* %Hp_Arg, i64 %lndFi, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
udwe:
  %lndFj = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdvF_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndFk = load i64*, i64** %Sp_Var
  %lndFl = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndFj( i64* %Base_Arg, i64* %lndFk, i64* %Hp_Arg, i64 %lndFl, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdvF_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdvF_info$def to i8*)
define internal ghccc void @cdvF_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdvF_info$def to i64)),i64 176), i64 1415, i64 324910088349286432}>
{
cdvF:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsbaa = alloca i64, i32 1
  %lsbab = alloca i64, i32 1
  %lsba5 = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lndFm = load i64*, i64** %Hp_Var
  %lndFn = getelementptr inbounds i64, i64* %lndFm, i32 9
  %lndFo = ptrtoint i64* %lndFn to i64
  %lndFp = inttoptr i64 %lndFo to i64*
  store i64* %lndFp, i64** %Hp_Var
  %lndFq = load i64*, i64** %Hp_Var
  %lndFr = ptrtoint i64* %lndFq to i64
  %lndFs = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lndFt = bitcast i64* %lndFs to i64*
  %lndFu = load i64, i64* %lndFt, !tbaa !4
  %lndFv = icmp ugt i64 %lndFr, %lndFu
  br i1 %lndFv, label %cdvP, label %cdvO
cdvO:
  %lndFy = load i64, i64* %R1_Var
  %lndFz = add i64 %lndFy, 7
  %lndFA = inttoptr i64 %lndFz to i64*
  %lndFB = load i64, i64* %lndFA, !tbaa !3
  store i64 %lndFB, i64* %lsbaa
  %lndFE = load i64, i64* %R1_Var
  %lndFF = add i64 %lndFE, 15
  %lndFG = inttoptr i64 %lndFF to i64*
  %lndFH = load i64, i64* %lndFG, !tbaa !3
  store i64 %lndFH, i64* %lsbab
  %lndFJ = ptrtoint i8* @ghczmprim_GHCziTuple_Z2T_con_info to i64
  %lndFI = load i64*, i64** %Hp_Var
  %lndFK = getelementptr inbounds i64, i64* %lndFI, i32 -8
  store i64 %lndFJ, i64* %lndFK, !tbaa !2
  %lndFM = load i64*, i64** %Sp_Var
  %lndFN = getelementptr inbounds i64, i64* %lndFM, i32 6
  %lndFO = bitcast i64* %lndFN to i64*
  %lndFP = load i64, i64* %lndFO, !tbaa !1
  %lndFL = load i64*, i64** %Hp_Var
  %lndFQ = getelementptr inbounds i64, i64* %lndFL, i32 -7
  store i64 %lndFP, i64* %lndFQ, !tbaa !2
  %lndFS = load i64*, i64** %Sp_Var
  %lndFT = getelementptr inbounds i64, i64* %lndFS, i32 7
  %lndFU = bitcast i64* %lndFT to i64*
  %lndFV = load i64, i64* %lndFU, !tbaa !1
  %lndFR = load i64*, i64** %Hp_Var
  %lndFW = getelementptr inbounds i64, i64* %lndFR, i32 -6
  store i64 %lndFV, i64* %lndFW, !tbaa !2
  %lndFY = ptrtoint i8* @ghczmprim_GHCziTypes_ZC_con_info to i64
  %lndFX = load i64*, i64** %Hp_Var
  %lndFZ = getelementptr inbounds i64, i64* %lndFX, i32 -5
  store i64 %lndFY, i64* %lndFZ, !tbaa !2
  %lndG2 = load i64*, i64** %Hp_Var
  %lndG3 = ptrtoint i64* %lndG2 to i64
  %lndG4 = add i64 %lndG3, -63
  %lndG0 = load i64*, i64** %Hp_Var
  %lndG5 = getelementptr inbounds i64, i64* %lndG0, i32 -4
  store i64 %lndG4, i64* %lndG5, !tbaa !2
  %lndG7 = load i64, i64* %lsbaa
  %lndG6 = load i64*, i64** %Hp_Var
  %lndG8 = getelementptr inbounds i64, i64* %lndG6, i32 -3
  store i64 %lndG7, i64* %lndG8, !tbaa !2
  %lndGa = ptrtoint i8* @Disk_FL_con_info to i64
  %lndG9 = load i64*, i64** %Hp_Var
  %lndGb = getelementptr inbounds i64, i64* %lndG9, i32 -2
  store i64 %lndGa, i64* %lndGb, !tbaa !2
  %lndGe = load i64*, i64** %Hp_Var
  %lndGf = ptrtoint i64* %lndGe to i64
  %lndGg = add i64 %lndGf, -38
  %lndGc = load i64*, i64** %Hp_Var
  %lndGh = getelementptr inbounds i64, i64* %lndGc, i32 -1
  store i64 %lndGg, i64* %lndGh, !tbaa !2
  %lndGj = load i64, i64* %lsbab
  %lndGi = load i64*, i64** %Hp_Var
  %lndGk = getelementptr inbounds i64, i64* %lndGi, i32 0
  store i64 %lndGj, i64* %lndGk, !tbaa !2
  %lndGl = load i64*, i64** %Sp_Var
  %lndGm = getelementptr inbounds i64, i64* %lndGl, i32 1
  %lndGn = bitcast i64* %lndGm to i64*
  %lndGo = load i64, i64* %lndGn, !tbaa !1
  store i64 %lndGo, i64* %lsba5
  %lndGp = load i64, i64* %lsba5
  %lndGq = add i64 %lndGp, 8
  %lndGs = load i64*, i64** %Hp_Var
  %lndGt = ptrtoint i64* %lndGs to i64
  %lndGu = add i64 %lndGt, -15
  %lndGv = inttoptr i64 %lndGq to i64*
  store i64 %lndGu, i64* %lndGv, !tbaa !5
  %lndGw = ptrtoint i64* %Base_Arg to i64
  %lndGx = inttoptr i64 %lndGw to i8*
  %lndGy = load i64, i64* %lsba5
  %lndGz = inttoptr i64 %lndGy to i8*
  %lndGA = bitcast i8* @dirty_MUT_VAR to void (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  call ccc void (i8*, i8*) %lndGA( i8* %lndGx, i8* %lndGz ) nounwind
  %lndGB = load i64*, i64** %Sp_Var
  %lndGC = getelementptr inbounds i64, i64* %lndGB, i32 2
  %lndGD = ptrtoint i64* %lndGC to i64
  %lndGE = inttoptr i64 %lndGD to i64*
  store i64* %lndGE, i64** %Sp_Var
  %lndGF = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb9x_entry$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndGG = load i64*, i64** %Sp_Var
  %lndGH = load i64*, i64** %Hp_Var
  %lndGI = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndGF( i64* %Base_Arg, i64* %lndGG, i64* %lndGH, i64 %lndGI, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdvP:
  %lndGJ = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 72, i64* %lndGJ, !tbaa !4
  %lndGK = load i64, i64* %R1_Var
  store i64 %lndGK, i64* %R1_Var
  %lndGL = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndGM = load i64*, i64** %Sp_Var
  %lndGN = load i64*, i64** %Hp_Var
  %lndGO = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndGL( i64* %Base_Arg, i64* %lndGM, i64* %lndGN, i64 %lndGO, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@sb9x_entry = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb9x_entry$def to i8*)
define internal ghccc void @sb9x_entry$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind
{
sb9x:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lndGQ = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdtA_info$def to i64
  %lndGP = load i64*, i64** %Sp_Var
  %lndGR = getelementptr inbounds i64, i64* %lndGP, i32 -1
  store i64 %lndGQ, i64* %lndGR, !tbaa !1
  %lndGS = load i64*, i64** %Sp_Var
  %lndGT = getelementptr inbounds i64, i64* %lndGS, i32 4
  %lndGU = bitcast i64* %lndGT to i64*
  %lndGV = load i64, i64* %lndGU, !tbaa !1
  store i64 %lndGV, i64* %R3_Var
  %lndGW = ptrtoint %Disk_readzudisk4_closure_struct* @Disk_readzudisk4_closure$def to i64
  %lndGX = add i64 %lndGW, 1
  store i64 %lndGX, i64* %R2_Var
  %lndGY = load i64*, i64** %Sp_Var
  %lndGZ = getelementptr inbounds i64, i64* %lndGY, i32 -1
  %lndH0 = ptrtoint i64* %lndGZ to i64
  %lndH1 = inttoptr i64 %lndH0 to i64*
  store i64* %lndH1, i64** %Sp_Var
  %lndH2 = bitcast i8* @integerzmgmp_GHCziIntegerziType_timesInteger_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndH3 = load i64*, i64** %Sp_Var
  %lndH4 = load i64, i64* %R2_Var
  %lndH5 = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndH2( i64* %Base_Arg, i64* %lndH3, i64* %Hp_Arg, i64 %R1_Arg, i64 %lndH4, i64 %lndH5, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdtA_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdtA_info$def to i8*)
define internal ghccc void @cdtA_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdtA_info$def to i64)),i64 176), i64 2758, i64 324892496163242016}>
{
cdtA:
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %lndH6 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdtE_info$def to i64
  %lndH7 = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lndH6, i64* %lndH7, !tbaa !1
  store i64 %R1_Arg, i64* %R2_Var
  %lndH8 = bitcast i8* @integerzmgmp_GHCziIntegerziType_integerToInt_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndH9 = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndH8( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lndH9, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdtE_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdtE_info$def to i8*)
define internal ghccc void @cdtE_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdtE_info$def to i64)),i64 176), i64 2758, i64 324892496163242016}>
{
cdtE:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %lsb9E = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %lndHa = getelementptr inbounds i64, i64* %Sp_Arg, i32 1
  %lndHb = bitcast i64* %lndHa to i64*
  %lndHc = load i64, i64* %lndHb, !tbaa !1
  %lndHd = bitcast i8* @ghczuwrapperZC23ZCbaseZCSystemziPosixziInternalsZClseek to i64 (i64, i64, i64)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lndHe = call ccc i64 (i64, i64, i64) %lndHd( i64 %lndHc, i64 %R1_Arg, i64 0 ) nounwind
  store i64 %lndHe, i64* %lsb9E
  %lndHf = load i64, i64* %lsb9E
  switch i64 %lndHf, label %udw7 [i64 -1, label %cdvo]
udw7:
  %lndHg = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdtP_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndHg( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdvo:
  %lndHh = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdvn_info$def to i64
  %lndHi = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lndHh, i64* %lndHi, !tbaa !1
  %lndHj = ptrtoint i8* @unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_fdSeek2_closure to i64
  store i64 %lndHj, i64* %R2_Var
  %lndHk = bitcast i8* @base_ForeignziCziError_throwErrno1_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndHl = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndHk( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lndHl, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdvn_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdvn_info$def to i8*)
define internal ghccc void @cdvn_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdvn_info$def to i64)),i64 312), i64 2758, i64 2478196129824}>
{
cdvn:
  %lndHm = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdtP_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndHm( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdtP_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdtP_info$def to i8*)
define internal ghccc void @cdtP_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdtP_info$def to i64)),i64 312), i64 2758, i64 2478196129824}>
{
cdtP:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lndHn = load i64*, i64** %Hp_Var
  %lndHo = getelementptr inbounds i64, i64* %lndHn, i32 4
  %lndHp = ptrtoint i64* %lndHo to i64
  %lndHq = inttoptr i64 %lndHp to i64*
  store i64* %lndHq, i64** %Hp_Var
  %lndHr = load i64*, i64** %Hp_Var
  %lndHs = ptrtoint i64* %lndHr to i64
  %lndHt = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lndHu = bitcast i64* %lndHt to i64*
  %lndHv = load i64, i64* %lndHu, !tbaa !4
  %lndHw = icmp ugt i64 %lndHs, %lndHv
  br i1 %lndHw, label %cduW, label %cduV
cduV:
  %lndHy = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @sb9P_info$def to i64
  %lndHx = load i64*, i64** %Hp_Var
  %lndHz = getelementptr inbounds i64, i64* %lndHx, i32 -3
  store i64 %lndHy, i64* %lndHz, !tbaa !2
  %lndHB = load i64*, i64** %Sp_Var
  %lndHC = getelementptr inbounds i64, i64* %lndHB, i32 4
  %lndHD = bitcast i64* %lndHC to i64*
  %lndHE = load i64, i64* %lndHD, !tbaa !1
  %lndHA = load i64*, i64** %Hp_Var
  %lndHF = getelementptr inbounds i64, i64* %lndHA, i32 -2
  store i64 %lndHE, i64* %lndHF, !tbaa !2
  %lndHH = load i64*, i64** %Sp_Var
  %lndHI = getelementptr inbounds i64, i64* %lndHH, i32 2
  %lndHJ = bitcast i64* %lndHI to i64*
  %lndHK = load i64, i64* %lndHJ, !tbaa !1
  %lndHG = load i64*, i64** %Hp_Var
  %lndHL = getelementptr inbounds i64, i64* %lndHG, i32 -1
  store i64 %lndHK, i64* %lndHL, !tbaa !2
  %lndHN = load i64*, i64** %Sp_Var
  %lndHO = getelementptr inbounds i64, i64* %lndHN, i32 1
  %lndHP = bitcast i64* %lndHO to i64*
  %lndHQ = load i64, i64* %lndHP, !tbaa !1
  %lndHM = load i64*, i64** %Hp_Var
  %lndHR = getelementptr inbounds i64, i64* %lndHM, i32 0
  store i64 %lndHQ, i64* %lndHR, !tbaa !2
  %lndHT = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cduR_info$def to i64
  %lndHS = load i64*, i64** %Sp_Var
  %lndHU = getelementptr inbounds i64, i64* %lndHS, i32 2
  store i64 %lndHT, i64* %lndHU, !tbaa !1
  %lndHW = load i64*, i64** %Hp_Var
  %lndHX = ptrtoint i64* %lndHW to i64
  %lndHY = add i64 %lndHX, -23
  store i64 %lndHY, i64* %R4_Var
  %lndHZ = ptrtoint i8* @unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_fdWrite2_closure to i64
  store i64 %lndHZ, i64* %R3_Var
  %lndI0 = ptrtoint i8* @unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_fdRead9_closure to i64
  %lndI1 = add i64 %lndI0, 1
  store i64 %lndI1, i64* %R2_Var
  %lndI2 = load i64*, i64** %Sp_Var
  %lndI3 = getelementptr inbounds i64, i64* %lndI2, i32 2
  %lndI4 = ptrtoint i64* %lndI3 to i64
  %lndI5 = inttoptr i64 %lndI4 to i64*
  store i64* %lndI5, i64** %Sp_Var
  %lndI6 = bitcast i8* @base_ForeignziCziError_throwErrnoIfMinus1Retry2_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndI7 = load i64*, i64** %Sp_Var
  %lndI8 = load i64*, i64** %Hp_Var
  %lndI9 = load i64, i64* %R2_Var
  %lndIa = load i64, i64* %R3_Var
  %lndIb = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndI6( i64* %Base_Arg, i64* %lndI7, i64* %lndI8, i64 %R1_Arg, i64 %lndI9, i64 %lndIa, i64 %lndIb, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cduW:
  %lndIc = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 32, i64* %lndIc, !tbaa !4
  %lndIe = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdtP_info$def to i64
  %lndId = load i64*, i64** %Sp_Var
  %lndIf = getelementptr inbounds i64, i64* %lndId, i32 0
  store i64 %lndIe, i64* %lndIf, !tbaa !1
  %lndIg = bitcast i8* @stg_gc_noregs to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndIh = load i64*, i64** %Sp_Var
  %lndIi = load i64*, i64** %Hp_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndIg( i64* %Base_Arg, i64* %lndIh, i64* %lndIi, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cduR_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cduR_info$def to i8*)
define internal ghccc void @cduR_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cduR_info$def to i64)),i64 360), i64 644, i64 4294967328}>
{
cduR:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lndIj = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cduT_info$def to i64
  %lndIk = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lndIj, i64* %lndIk, !tbaa !1
  %lndIl = load i64, i64* %R1_Var
  store i64 %lndIl, i64* %R1_Var
  %lndIm = load i64, i64* %R1_Var
  %lndIn = and i64 %lndIm, 7
  %lndIo = icmp ne i64 %lndIn, 0
  br i1 %lndIo, label %udwc, label %cduY
cduY:
  %lndIq = load i64, i64* %R1_Var
  %lndIr = inttoptr i64 %lndIq to i64*
  %lndIs = load i64, i64* %lndIr, !tbaa !3
  %lndIt = inttoptr i64 %lndIs to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndIu = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndIt( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lndIu, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
udwc:
  %lndIv = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cduT_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndIw = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndIv( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lndIw, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cduT_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cduT_info$def to i8*)
define internal ghccc void @cduT_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cduT_info$def to i64)),i64 360), i64 644, i64 4294967328}>
{
cduT:
  %lsb9V = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lndIz = load i64, i64* %R1_Var
  %lndIA = add i64 %lndIz, 7
  %lndIB = inttoptr i64 %lndIA to i64*
  %lndIC = load i64, i64* %lndIB, !tbaa !3
  store i64 %lndIC, i64* %lsb9V
  %lndID = load i64, i64* %lsb9V
  switch i64 %lndID, label %cdv8 [i64 4096, label %cdve]
cdv8:
  %lndIE = load i64, i64* %lsb9V
  store i64 %lndIE, i64* %R3_Var
  %lndIF = load i64*, i64** %Sp_Var
  %lndIG = getelementptr inbounds i64, i64* %lndIF, i32 3
  %lndIH = bitcast i64* %lndIG to i64*
  %lndII = load i64, i64* %lndIH, !tbaa !1
  store i64 %lndII, i64* %R2_Var
  %lndIJ = load i64*, i64** %Sp_Var
  %lndIK = getelementptr inbounds i64, i64* %lndIJ, i32 5
  %lndIL = ptrtoint i64* %lndIK to i64
  %lndIM = inttoptr i64 %lndIL to i64*
  store i64* %lndIM, i64** %Sp_Var
  %lndIN = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rb0S_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndIO = load i64*, i64** %Sp_Var
  %lndIP = load i64, i64* %R1_Var
  %lndIQ = load i64, i64* %R2_Var
  %lndIR = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndIN( i64* %Base_Arg, i64* %lndIO, i64* %Hp_Arg, i64 %lndIP, i64 %lndIQ, i64 %lndIR, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdve:
  %lndIS = ptrtoint i8* @ghczmprim_GHCziTuple_Z0T_closure to i64
  %lndIT = add i64 %lndIS, 1
  store i64 %lndIT, i64* %R1_Var
  %lndIU = load i64*, i64** %Sp_Var
  %lndIV = getelementptr inbounds i64, i64* %lndIU, i32 5
  %lndIW = ptrtoint i64* %lndIV to i64
  %lndIX = inttoptr i64 %lndIW to i64*
  store i64* %lndIX, i64** %Sp_Var
  %lndIY = load i64*, i64** %Sp_Var
  %lndIZ = getelementptr inbounds i64, i64* %lndIY, i32 0
  %lndJ0 = bitcast i64* %lndIZ to i64*
  %lndJ1 = load i64, i64* %lndJ0, !tbaa !1
  %lndJ2 = inttoptr i64 %lndJ1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndJ3 = load i64*, i64** %Sp_Var
  %lndJ4 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndJ2( i64* %Base_Arg, i64* %lndJ3, i64* %Hp_Arg, i64 %lndJ4, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cduw_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cduw_info$def to i8*)
define internal ghccc void @cduw_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cduw_info$def to i64)),i64 176), i64 259, i64 973428434690637856}>
{
cduw:
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %lndJ5 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cduy_info$def to i64
  %lndJ6 = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lndJ5, i64* %lndJ6, !tbaa !1
  store i64 %R1_Arg, i64* %R2_Var
  %lndJ7 = bitcast i8* @Word_zdWWBS_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndJ8 = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndJ7( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lndJ8, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cduy_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cduy_info$def to i8*)
define internal ghccc void @cduy_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cduy_info$def to i64)),i64 176), i64 259, i64 973428434690637856}>
{
cduy:
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lndJa = load i64*, i64** %Sp_Var
  %lndJb = getelementptr inbounds i64, i64* %lndJa, i32 1
  %lndJc = bitcast i64* %lndJb to i64*
  %lndJd = load i64, i64* %lndJc, !tbaa !1
  %lndJ9 = load i64*, i64** %Sp_Var
  %lndJe = getelementptr inbounds i64, i64* %lndJ9, i32 1
  store i64 %lndJd, i64* %lndJe, !tbaa !1
  %lndJg = load i64*, i64** %Sp_Var
  %lndJh = getelementptr inbounds i64, i64* %lndJg, i32 2
  %lndJi = bitcast i64* %lndJh to i64*
  %lndJj = load i64, i64* %lndJi, !tbaa !1
  %lndJf = load i64*, i64** %Sp_Var
  %lndJk = getelementptr inbounds i64, i64* %lndJf, i32 2
  store i64 %lndJj, i64* %lndJk, !tbaa !1
  %lndJl = load i64*, i64** %Sp_Var
  %lndJm = getelementptr inbounds i64, i64* %lndJl, i32 3
  store i64 %R1_Arg, i64* %lndJm, !tbaa !1
  %lndJn = load i64*, i64** %Sp_Var
  %lndJo = getelementptr inbounds i64, i64* %lndJn, i32 1
  %lndJp = ptrtoint i64* %lndJo to i64
  %lndJq = inttoptr i64 %lndJp to i64*
  store i64* %lndJq, i64** %Sp_Var
  %lndJr = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdsX_entry$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndJs = load i64*, i64** %Sp_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndJr( i64* %Base_Arg, i64* %lndJs, i64* %Hp_Arg, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_writezudisk_closure_struct = type <{i64, i64}>
@Disk_writezudisk_closure$def = internal global %Disk_writezudisk_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_writezudisk_info$def to i64), i64 0}>
@Disk_writezudisk_closure = alias i8* bitcast (%Disk_writezudisk_closure_struct* @Disk_writezudisk_closure$def to i8*)
@Disk_writezudisk_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_writezudisk_info$def to i8*)
define ghccc void @Disk_writezudisk_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_writezudisk_info$def to i64)),i64 376), i64 17179869207, i64 0, i64 4294967311}>
{
cdJy:
  %R4_Var = alloca i64, i32 1
  store i64 %R4_Arg, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lndJB = load i64, i64* %R4_Var
  store i64 %lndJB, i64* %R4_Var
  %lndJC = load i64, i64* %R3_Var
  store i64 %lndJC, i64* %R3_Var
  %lndJD = load i64, i64* %R2_Var
  store i64 %lndJD, i64* %R2_Var
  %lndJE = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_writezudisk1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndJF = load i64, i64* %R2_Var
  %lndJG = load i64, i64* %R3_Var
  %lndJH = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndJE( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lndJF, i64 %lndJG, i64 %lndJH, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_synczudisk1_closure_struct = type <{i64, i64}>
@Disk_synczudisk1_closure$def = internal global %Disk_synczudisk1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_synczudisk1_info$def to i64), i64 0}>
@Disk_synczudisk1_closure = alias i8* bitcast (%Disk_synczudisk1_closure_struct* @Disk_synczudisk1_closure$def to i8*)
@Disk_synczudisk1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_synczudisk1_info$def to i8*)
define ghccc void @Disk_synczudisk1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_synczudisk1_info$def to i64)),i64 176), i64 8589934597, i64 0, i64 3458764518115508239}>
{
cdKu:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lndMy = load i64*, i64** %Sp_Var
  %lndMz = getelementptr inbounds i64, i64* %lndMy, i32 -4
  %lndMA = ptrtoint i64* %lndMz to i64
  %lndMB = icmp ult i64 %lndMA, %SpLim_Arg
  br i1 %lndMB, label %cdKy, label %cdKz
cdKz:
  %lndMD = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdKr_info$def to i64
  %lndMC = load i64*, i64** %Sp_Var
  %lndME = getelementptr inbounds i64, i64* %lndMC, i32 -1
  store i64 %lndMD, i64* %lndME, !tbaa !1
  %lndMF = load i64, i64* %R2_Var
  store i64 %lndMF, i64* %R1_Var
  %lndMG = load i64*, i64** %Sp_Var
  %lndMH = getelementptr inbounds i64, i64* %lndMG, i32 -1
  %lndMI = ptrtoint i64* %lndMH to i64
  %lndMJ = inttoptr i64 %lndMI to i64*
  store i64* %lndMJ, i64** %Sp_Var
  %lndMK = load i64, i64* %R1_Var
  %lndML = and i64 %lndMK, 7
  %lndMM = icmp ne i64 %lndML, 0
  br i1 %lndMM, label %udMs, label %cdKs
cdKs:
  %lndMO = load i64, i64* %R1_Var
  %lndMP = inttoptr i64 %lndMO to i64*
  %lndMQ = load i64, i64* %lndMP, !tbaa !3
  %lndMR = inttoptr i64 %lndMQ to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndMS = load i64*, i64** %Sp_Var
  %lndMT = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndMR( i64* %Base_Arg, i64* %lndMS, i64* %Hp_Arg, i64 %lndMT, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
udMs:
  %lndMU = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdKr_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndMV = load i64*, i64** %Sp_Var
  %lndMW = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndMU( i64* %Base_Arg, i64* %lndMV, i64* %Hp_Arg, i64 %lndMW, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdKy:
  %lndMX = load i64, i64* %R2_Var
  store i64 %lndMX, i64* %R2_Var
  %lndMY = ptrtoint %Disk_synczudisk1_closure_struct* @Disk_synczudisk1_closure$def to i64
  store i64 %lndMY, i64* %R1_Var
  %lndMZ = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lndN0 = bitcast i64* %lndMZ to i64*
  %lndN1 = load i64, i64* %lndN0, !tbaa !4
  %lndN2 = inttoptr i64 %lndN1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndN3 = load i64*, i64** %Sp_Var
  %lndN4 = load i64, i64* %R1_Var
  %lndN5 = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndN2( i64* %Base_Arg, i64* %lndN3, i64* %Hp_Arg, i64 %lndN4, i64 %lndN5, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdKr_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdKr_info$def to i8*)
define internal ghccc void @cdKr_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdKr_info$def to i64)),i64 176), i64 0, i64 1152921508901814304}>
{
cdKr:
  %lsbak = alloca i64, i32 1
  %lsbal = alloca i64, i32 1
  %lsbaj = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lndN7 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdKx_info$def to i64
  %lndN6 = load i64*, i64** %Sp_Var
  %lndN8 = getelementptr inbounds i64, i64* %lndN6, i32 -3
  store i64 %lndN7, i64* %lndN8, !tbaa !1
  %lndNb = load i64, i64* %R1_Var
  %lndNc = add i64 %lndNb, 7
  %lndNd = inttoptr i64 %lndNc to i64*
  %lndNe = load i64, i64* %lndNd, !tbaa !3
  store i64 %lndNe, i64* %lsbak
  %lndNh = load i64, i64* %R1_Var
  %lndNi = add i64 %lndNh, 15
  %lndNj = inttoptr i64 %lndNi to i64*
  %lndNk = load i64, i64* %lndNj, !tbaa !3
  store i64 %lndNk, i64* %lsbal
  %lndNn = load i64, i64* %R1_Var
  %lndNo = add i64 %lndNn, 31
  %lndNp = inttoptr i64 %lndNo to i64*
  %lndNq = load i64, i64* %lndNp, !tbaa !3
  store i64 %lndNq, i64* %lsbaj
  %lndNr = load i64, i64* %lsbak
  %lndNs = add i64 %lndNr, 8
  %lndNt = inttoptr i64 %lndNs to i64*
  %lndNu = load i64, i64* %lndNt, !tbaa !5
  store i64 %lndNu, i64* %R1_Var
  %lndNw = load i64, i64* %lsbak
  %lndNv = load i64*, i64** %Sp_Var
  %lndNx = getelementptr inbounds i64, i64* %lndNv, i32 -2
  store i64 %lndNw, i64* %lndNx, !tbaa !1
  %lndNz = load i64, i64* %lsbal
  %lndNy = load i64*, i64** %Sp_Var
  %lndNA = getelementptr inbounds i64, i64* %lndNy, i32 -1
  store i64 %lndNz, i64* %lndNA, !tbaa !1
  %lndNC = load i64, i64* %lsbaj
  %lndNB = load i64*, i64** %Sp_Var
  %lndND = getelementptr inbounds i64, i64* %lndNB, i32 0
  store i64 %lndNC, i64* %lndND, !tbaa !1
  %lndNE = load i64*, i64** %Sp_Var
  %lndNF = getelementptr inbounds i64, i64* %lndNE, i32 -3
  %lndNG = ptrtoint i64* %lndNF to i64
  %lndNH = inttoptr i64 %lndNG to i64*
  store i64* %lndNH, i64** %Sp_Var
  %lndNI = load i64, i64* %R1_Var
  %lndNJ = and i64 %lndNI, 7
  %lndNK = icmp ne i64 %lndNJ, 0
  br i1 %lndNK, label %udMr, label %cdKB
cdKB:
  %lndNM = load i64, i64* %R1_Var
  %lndNN = inttoptr i64 %lndNM to i64*
  %lndNO = load i64, i64* %lndNN, !tbaa !3
  %lndNP = inttoptr i64 %lndNO to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndNQ = load i64*, i64** %Sp_Var
  %lndNR = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndNP( i64* %Base_Arg, i64* %lndNQ, i64* %Hp_Arg, i64 %lndNR, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
udMr:
  %lndNS = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdKx_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndNT = load i64*, i64** %Sp_Var
  %lndNU = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndNS( i64* %Base_Arg, i64* %lndNT, i64* %Hp_Arg, i64 %lndNU, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdKx_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdKx_info$def to i8*)
define internal ghccc void @cdKx_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdKx_info$def to i64)),i64 176), i64 259, i64 1152921508901814304}>
{
cdKx:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsbar = alloca i64, i32 1
  %lsbas = alloca i64, i32 1
  %lsbau = alloca i64, i32 1
  %lsbak = alloca i64, i32 1
  %lsbal = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lndNV = load i64*, i64** %Hp_Var
  %lndNW = getelementptr inbounds i64, i64* %lndNV, i32 4
  %lndNX = ptrtoint i64* %lndNW to i64
  %lndNY = inttoptr i64 %lndNX to i64*
  store i64* %lndNY, i64** %Hp_Var
  %lndNZ = load i64*, i64** %Hp_Var
  %lndO0 = ptrtoint i64* %lndNZ to i64
  %lndO1 = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lndO2 = bitcast i64* %lndO1 to i64*
  %lndO3 = load i64, i64* %lndO2, !tbaa !4
  %lndO4 = icmp ugt i64 %lndO0, %lndO3
  br i1 %lndO4, label %cdKH, label %cdKG
cdKG:
  %lndO7 = load i64, i64* %R1_Var
  %lndO8 = add i64 %lndO7, 7
  %lndO9 = inttoptr i64 %lndO8 to i64*
  %lndOa = load i64, i64* %lndO9, !tbaa !3
  store i64 %lndOa, i64* %lsbar
  %lndOd = load i64, i64* %R1_Var
  %lndOe = add i64 %lndOd, 15
  %lndOf = inttoptr i64 %lndOe to i64*
  %lndOg = load i64, i64* %lndOf, !tbaa !3
  store i64 %lndOg, i64* %lsbas
  %lndOj = load i64, i64* %R1_Var
  %lndOk = add i64 %lndOj, 23
  %lndOl = inttoptr i64 %lndOk to i64*
  %lndOm = load i64, i64* %lndOl, !tbaa !3
  %lndOn = add i64 %lndOm, 1
  store i64 %lndOn, i64* %lsbau
  %lndOp = ptrtoint i8* @Disk_Stats_con_info to i64
  %lndOo = load i64*, i64** %Hp_Var
  %lndOq = getelementptr inbounds i64, i64* %lndOo, i32 -3
  store i64 %lndOp, i64* %lndOq, !tbaa !2
  %lndOs = load i64, i64* %lsbar
  %lndOr = load i64*, i64** %Hp_Var
  %lndOt = getelementptr inbounds i64, i64* %lndOr, i32 -2
  store i64 %lndOs, i64* %lndOt, !tbaa !2
  %lndOv = load i64, i64* %lsbas
  %lndOu = load i64*, i64** %Hp_Var
  %lndOw = getelementptr inbounds i64, i64* %lndOu, i32 -1
  store i64 %lndOv, i64* %lndOw, !tbaa !2
  %lndOy = load i64, i64* %lsbau
  %lndOx = load i64*, i64** %Hp_Var
  %lndOz = getelementptr inbounds i64, i64* %lndOx, i32 0
  store i64 %lndOy, i64* %lndOz, !tbaa !2
  %lndOA = load i64*, i64** %Sp_Var
  %lndOB = getelementptr inbounds i64, i64* %lndOA, i32 1
  %lndOC = bitcast i64* %lndOB to i64*
  %lndOD = load i64, i64* %lndOC, !tbaa !1
  store i64 %lndOD, i64* %lsbak
  %lndOE = load i64, i64* %lsbak
  %lndOF = add i64 %lndOE, 8
  %lndOH = load i64*, i64** %Hp_Var
  %lndOI = ptrtoint i64* %lndOH to i64
  %lndOJ = add i64 %lndOI, -23
  %lndOK = inttoptr i64 %lndOF to i64*
  store i64 %lndOJ, i64* %lndOK, !tbaa !5
  %lndOL = load i64*, i64** %Sp_Var
  %lndOM = getelementptr inbounds i64, i64* %lndOL, i32 2
  %lndON = bitcast i64* %lndOM to i64*
  %lndOO = load i64, i64* %lndON, !tbaa !1
  store i64 %lndOO, i64* %lsbal
  %lndOP = ptrtoint i64* %Base_Arg to i64
  %lndOQ = inttoptr i64 %lndOP to i8*
  %lndOR = load i64, i64* %lsbak
  %lndOS = inttoptr i64 %lndOR to i8*
  %lndOT = bitcast i8* @dirty_MUT_VAR to void (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  call ccc void (i8*, i8*) %lndOT( i8* %lndOQ, i8* %lndOS ) nounwind
  %lndOV = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdKR_info$def to i64
  %lndOU = load i64*, i64** %Sp_Var
  %lndOW = getelementptr inbounds i64, i64* %lndOU, i32 2
  store i64 %lndOV, i64* %lndOW, !tbaa !1
  %lndOX = load i64, i64* %lsbal
  store i64 %lndOX, i64* %R1_Var
  %lndOY = load i64*, i64** %Sp_Var
  %lndOZ = getelementptr inbounds i64, i64* %lndOY, i32 2
  %lndP0 = ptrtoint i64* %lndOZ to i64
  %lndP1 = inttoptr i64 %lndP0 to i64*
  store i64* %lndP1, i64** %Sp_Var
  %lndP2 = load i64, i64* %R1_Var
  %lndP3 = and i64 %lndP2, 7
  %lndP4 = icmp ne i64 %lndP3, 0
  br i1 %lndP4, label %udMt, label %cdKS
cdKS:
  %lndP6 = load i64, i64* %R1_Var
  %lndP7 = inttoptr i64 %lndP6 to i64*
  %lndP8 = load i64, i64* %lndP7, !tbaa !3
  %lndP9 = inttoptr i64 %lndP8 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndPa = load i64*, i64** %Sp_Var
  %lndPb = load i64*, i64** %Hp_Var
  %lndPc = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndP9( i64* %Base_Arg, i64* %lndPa, i64* %lndPb, i64 %lndPc, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
udMt:
  %lndPd = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdKR_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndPe = load i64*, i64** %Sp_Var
  %lndPf = load i64*, i64** %Hp_Var
  %lndPg = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndPd( i64* %Base_Arg, i64* %lndPe, i64* %lndPf, i64 %lndPg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdKH:
  %lndPh = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 32, i64* %lndPh, !tbaa !4
  %lndPi = load i64, i64* %R1_Var
  store i64 %lndPi, i64* %R1_Var
  %lndPj = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndPk = load i64*, i64** %Sp_Var
  %lndPl = load i64*, i64** %Hp_Var
  %lndPm = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndPj( i64* %Base_Arg, i64* %lndPk, i64* %lndPl, i64 %lndPm, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdKR_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdKR_info$def to i8*)
define internal ghccc void @cdKR_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdKR_info$def to i64)),i64 176), i64 65, i64 1152921508901814304}>
{
cdKR:
  %lcdL0 = alloca i64, i32 1
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %ludMe = alloca i64, i32 1
  %ludMf = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %ludMb = alloca i64, i32 1
  %lsbaB = alloca i64, i32 1
  %ludMc = alloca i64, i32 1
  %Base_Var = alloca i64*, i32 1
  store i64* %Base_Arg, i64** %Base_Var
  %ludMd = alloca i64, i32 1
  %SpLim_Var = alloca i64, i32 1
  store i64 %SpLim_Arg, i64* %SpLim_Var
  %ludMg = alloca i64, i32 1
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %ludMh = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lndPn = load i64, i64* %R1_Var
  %lndPo = and i64 %lndPn, 7
  switch i64 %lndPo, label %cdKY [i64 1, label %cdKY
i64 2, label %cdLv]
cdKY:
  %lndPp = load i64*, i64** %Sp_Var
  %lndPq = getelementptr inbounds i64, i64* %lndPp, i32 1
  %lndPr = bitcast i64* %lndPq to i64*
  %lndPs = load i64, i64* %lndPr, !tbaa !1
  store i64 %lndPs, i64* %lcdL0
  %lndPu = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdL1_info$def to i64
  %lndPt = load i64*, i64** %Sp_Var
  %lndPv = getelementptr inbounds i64, i64* %lndPt, i32 1
  store i64 %lndPu, i64* %lndPv, !tbaa !1
  %lndPw = load i64*, i64** %Sp_Var
  %lndPx = getelementptr inbounds i64, i64* %lndPw, i32 1
  %lndPy = ptrtoint i64* %lndPx to i64
  %lndPz = inttoptr i64 %lndPy to i64*
  store i64* %lndPz, i64** %Sp_Var
  %lndPA = load i64*, i64** %Base_Var
  %lndPB = getelementptr inbounds i64, i64* %lndPA, i32 109
  %lndPC = bitcast i64* %lndPB to i64*
  %lndPD = load i64, i64* %lndPC, !tbaa !4
  store i64 %lndPD, i64* %ludMe
  %lndPE = load i64, i64* %ludMe
  %lndPF = add i64 %lndPE, 24
  %lndPG = inttoptr i64 %lndPF to i64*
  %lndPH = load i64, i64* %lndPG, !tbaa !5
  %lndPI = add i64 %lndPH, 16
  %lndPJ = load i64*, i64** %Sp_Var
  %lndPK = ptrtoint i64* %lndPJ to i64
  %lndPL = inttoptr i64 %lndPI to i64*
  store i64 %lndPK, i64* %lndPL, !tbaa !5
  %lndPM = load i64*, i64** %Base_Var
  %lndPN = getelementptr inbounds i64, i64* %lndPM, i32 111
  %lndPO = bitcast i64* %lndPN to i64*
  %lndPP = load i64, i64* %lndPO, !tbaa !4
  store i64 %lndPP, i64* %ludMf
  %lndPQ = load i64, i64* %ludMf
  %lndPR = add i64 %lndPQ, 8
  %lndPS = load i64*, i64** %Hp_Var
  %lndPT = getelementptr inbounds i64, i64* %lndPS, i32 1
  %lndPU = ptrtoint i64* %lndPT to i64
  %lndPV = inttoptr i64 %lndPR to i64*
  store i64 %lndPU, i64* %lndPV, !tbaa !5
  %lndPW = load i64, i64* %ludMe
  %lndPX = add i64 %lndPW, 104
  %lndPY = load i64, i64* %ludMe
  %lndPZ = add i64 %lndPY, 104
  %lndQ0 = inttoptr i64 %lndPZ to i64*
  %lndQ1 = load i64, i64* %lndQ0, !tbaa !5
  %lndQ2 = load i64*, i64** %Hp_Var
  %lndQ3 = getelementptr inbounds i64, i64* %lndQ2, i32 1
  %lndQ4 = ptrtoint i64* %lndQ3 to i64
  %lndQ5 = load i64, i64* %ludMf
  %lndQ6 = inttoptr i64 %lndQ5 to i64*
  %lndQ7 = load i64, i64* %lndQ6, !tbaa !5
  %lndQ8 = sub i64 %lndQ4, %lndQ7
  %lndQ9 = sub i64 %lndQ1, %lndQ8
  %lndQa = inttoptr i64 %lndPX to i64*
  store i64 %lndQ9, i64* %lndQa, !tbaa !5
  %lndQb = load i64*, i64** %Base_Var
  %lndQc = ptrtoint i64* %lndQb to i64
  %lndQd = inttoptr i64 %lndQc to i8*
  %lndQe = bitcast i8* @suspendThread to i8* (i8*, i64)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lndQf = call ccc i8* (i8*, i64) %lndQe( i8* %lndQd, i64 0 ) nounwind
  %lndQg = ptrtoint i8* %lndQf to i64
  store i64 %lndQg, i64* %ludMb
  %lndQh = load i64, i64* %lcdL0
  %lndQi = bitcast i8* @ghczuwrapperZC1ZCunixzm2zi7zi2zi0ZCSystemziPosixziUnistdZCfsync to i64 (i64)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lndQj = call ccc i64 (i64) %lndQi( i64 %lndQh ) nounwind
  store i64 %lndQj, i64* %lsbaB
  %lndQk = load i64, i64* %ludMb
  %lndQl = inttoptr i64 %lndQk to i8*
  %lndQm = bitcast i8* @resumeThread to i8* (i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lndQn = call ccc i8* (i8*) %lndQm( i8* %lndQl ) nounwind
  %lndQo = ptrtoint i8* %lndQn to i64
  store i64 %lndQo, i64* %ludMc
  %lndQp = load i64, i64* %ludMc
  %lndQq = inttoptr i64 %lndQp to i64*
  store i64* %lndQq, i64** %Base_Var
  %lndQr = load i64*, i64** %Base_Var
  %lndQs = getelementptr inbounds i64, i64* %lndQr, i32 109
  %lndQt = bitcast i64* %lndQs to i64*
  %lndQu = load i64, i64* %lndQt, !tbaa !4
  store i64 %lndQu, i64* %ludMe
  %lndQv = load i64, i64* %ludMe
  %lndQw = add i64 %lndQv, 24
  %lndQx = inttoptr i64 %lndQw to i64*
  %lndQy = load i64, i64* %lndQx, !tbaa !5
  store i64 %lndQy, i64* %ludMd
  %lndQz = load i64, i64* %ludMd
  %lndQA = add i64 %lndQz, 16
  %lndQB = inttoptr i64 %lndQA to i64*
  %lndQC = load i64, i64* %lndQB, !tbaa !5
  %lndQD = inttoptr i64 %lndQC to i64*
  store i64* %lndQD, i64** %Sp_Var
  %lndQE = load i64, i64* %ludMd
  %lndQF = add i64 %lndQE, 192
  store i64 %lndQF, i64* %SpLim_Var
  %lndQG = load i64*, i64** %Base_Var
  %lndQH = getelementptr inbounds i64, i64* %lndQG, i32 113
  store i64 0, i64* %lndQH, !tbaa !4
  %lndQI = load i64*, i64** %Base_Var
  %lndQJ = getelementptr inbounds i64, i64* %lndQI, i32 111
  %lndQK = bitcast i64* %lndQJ to i64*
  %lndQL = load i64, i64* %lndQK, !tbaa !4
  store i64 %lndQL, i64* %ludMf
  %lndQM = load i64, i64* %ludMf
  %lndQN = add i64 %lndQM, 8
  %lndQO = inttoptr i64 %lndQN to i64*
  %lndQP = load i64, i64* %lndQO, !tbaa !5
  store i64 %lndQP, i64* %ludMg
  %lndQQ = load i64, i64* %ludMg
  %lndQR = add i64 %lndQQ, -8
  %lndQS = inttoptr i64 %lndQR to i64*
  store i64* %lndQS, i64** %Hp_Var
  %lndQT = load i64, i64* %ludMf
  %lndQU = inttoptr i64 %lndQT to i64*
  %lndQV = load i64, i64* %lndQU, !tbaa !5
  store i64 %lndQV, i64* %ludMh
  %lndQX = load i64, i64* %ludMh
  %lndQY = load i64, i64* %ludMf
  %lndQZ = add i64 %lndQY, 48
  %lndR0 = inttoptr i64 %lndQZ to i32*
  %lndR1 = load i32, i32* %lndR0, !tbaa !5
  %lndR2 = sext i32 %lndR1 to i64
  %lndR3 = shl i64 %lndR2, 12
  %lndR4 = add i64 %lndR3, -1
  %lndR5 = add i64 %lndQX, %lndR4
  %lndQW = load i64*, i64** %Base_Var
  %lndR6 = getelementptr inbounds i64, i64* %lndQW, i32 107
  store i64 %lndR5, i64* %lndR6, !tbaa !4
  %lndR7 = load i64, i64* %ludMe
  %lndR8 = add i64 %lndR7, 104
  %lndR9 = load i64, i64* %ludMe
  %lndRa = add i64 %lndR9, 104
  %lndRb = inttoptr i64 %lndRa to i64*
  %lndRc = load i64, i64* %lndRb, !tbaa !5
  %lndRd = load i64, i64* %ludMg
  %lndRe = load i64, i64* %ludMh
  %lndRf = sub i64 %lndRd, %lndRe
  %lndRg = add i64 %lndRc, %lndRf
  %lndRh = inttoptr i64 %lndR8 to i64*
  store i64 %lndRg, i64* %lndRh, !tbaa !5
  %lndRi = load i64, i64* %lsbaB
  store i64 %lndRi, i64* %R1_Var
  %lndRj = load i64*, i64** %Sp_Var
  %lndRk = getelementptr inbounds i64, i64* %lndRj, i32 0
  %lndRl = bitcast i64* %lndRk to i64*
  %lndRm = load i64, i64* %lndRl, !tbaa !1
  %lndRn = inttoptr i64 %lndRm to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndRo = load i64*, i64** %Base_Var
  %lndRp = load i64*, i64** %Sp_Var
  %lndRq = load i64*, i64** %Hp_Var
  %lndRr = load i64, i64* %R1_Var
  %lndRs = load i64, i64* %SpLim_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndRn( i64* %lndRo, i64* %lndRp, i64* %lndRq, i64 %lndRr, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %lndRs ) nounwind
  ret void
cdLv:
  %lndRu = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdLo_info$def to i64
  %lndRt = load i64*, i64** %Sp_Var
  %lndRv = getelementptr inbounds i64, i64* %lndRt, i32 0
  store i64 %lndRu, i64* %lndRv, !tbaa !1
  %lndRy = load i64, i64* %R1_Var
  %lndRz = add i64 %lndRy, 6
  %lndRA = inttoptr i64 %lndRz to i64*
  %lndRB = load i64, i64* %lndRA, !tbaa !3
  store i64 %lndRB, i64* %R1_Var
  %lndRC = load i64, i64* %R1_Var
  %lndRD = and i64 %lndRC, 7
  %lndRE = icmp ne i64 %lndRD, 0
  br i1 %lndRE, label %udMu, label %cdLp
cdLp:
  %lndRG = load i64, i64* %R1_Var
  %lndRH = inttoptr i64 %lndRG to i64*
  %lndRI = load i64, i64* %lndRH, !tbaa !3
  %lndRJ = inttoptr i64 %lndRI to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndRK = load i64*, i64** %Base_Var
  %lndRL = load i64*, i64** %Sp_Var
  %lndRM = load i64*, i64** %Hp_Var
  %lndRN = load i64, i64* %R1_Var
  %lndRO = load i64, i64* %SpLim_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndRJ( i64* %lndRK, i64* %lndRL, i64* %lndRM, i64 %lndRN, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %lndRO ) nounwind
  ret void
udMu:
  %lndRP = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdLo_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndRQ = load i64*, i64** %Base_Var
  %lndRR = load i64*, i64** %Sp_Var
  %lndRS = load i64*, i64** %Hp_Var
  %lndRT = load i64, i64* %R1_Var
  %lndRU = load i64, i64* %SpLim_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndRP( i64* %lndRQ, i64* %lndRR, i64* %lndRS, i64 %lndRT, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %lndRU ) nounwind
  ret void
}
@cdLo_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdLo_info$def to i8*)
define internal ghccc void @cdLo_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdLo_info$def to i64)),i64 176), i64 65, i64 1152921508901814304}>
{
cdLo:
  %lsbaI = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lndRW = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdLt_info$def to i64
  %lndRV = load i64*, i64** %Sp_Var
  %lndRX = getelementptr inbounds i64, i64* %lndRV, i32 -1
  store i64 %lndRW, i64* %lndRX, !tbaa !1
  %lndS0 = load i64, i64* %R1_Var
  %lndS1 = add i64 %lndS0, 7
  %lndS2 = inttoptr i64 %lndS1 to i64*
  %lndS3 = load i64, i64* %lndS2, !tbaa !3
  store i64 %lndS3, i64* %lsbaI
  %lndS4 = load i64, i64* %lsbaI
  %lndS5 = add i64 %lndS4, 8
  %lndS6 = inttoptr i64 %lndS5 to i64*
  %lndS7 = load i64, i64* %lndS6, !tbaa !5
  store i64 %lndS7, i64* %R1_Var
  %lndS9 = load i64, i64* %lsbaI
  %lndS8 = load i64*, i64** %Sp_Var
  %lndSa = getelementptr inbounds i64, i64* %lndS8, i32 0
  store i64 %lndS9, i64* %lndSa, !tbaa !1
  %lndSb = load i64*, i64** %Sp_Var
  %lndSc = getelementptr inbounds i64, i64* %lndSb, i32 -1
  %lndSd = ptrtoint i64* %lndSc to i64
  %lndSe = inttoptr i64 %lndSd to i64*
  store i64* %lndSe, i64** %Sp_Var
  %lndSf = load i64, i64* %R1_Var
  %lndSg = and i64 %lndSf, 7
  %lndSh = icmp ne i64 %lndSg, 0
  br i1 %lndSh, label %udMw, label %cdLx
cdLx:
  %lndSj = load i64, i64* %R1_Var
  %lndSk = inttoptr i64 %lndSj to i64*
  %lndSl = load i64, i64* %lndSk, !tbaa !3
  %lndSm = inttoptr i64 %lndSl to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndSn = load i64*, i64** %Sp_Var
  %lndSo = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndSm( i64* %Base_Arg, i64* %lndSn, i64* %Hp_Arg, i64 %lndSo, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
udMw:
  %lndSp = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdLt_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndSq = load i64*, i64** %Sp_Var
  %lndSr = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndSp( i64* %Base_Arg, i64* %lndSq, i64* %Hp_Arg, i64 %lndSr, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdLt_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdLt_info$def to i8*)
define internal ghccc void @cdLt_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdLt_info$def to i64)),i64 176), i64 130, i64 1152921508901814304}>
{
cdLt:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lsbaN = alloca i64, i32 1
  %lsbaO = alloca i64, i32 1
  %lsbaI = alloca i64, i32 1
  %lsbaj = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R6_Var = alloca i64, i32 1
  store i64 undef, i64* %R6_Var
  %F1_Var = alloca float, i32 1
  store float undef, float* %F1_Var
  %D1_Var = alloca double, i32 1
  store double undef, double* %D1_Var
  %F2_Var = alloca float, i32 1
  store float undef, float* %F2_Var
  %D2_Var = alloca double, i32 1
  store double undef, double* %D2_Var
  %F3_Var = alloca float, i32 1
  store float undef, float* %F3_Var
  %D3_Var = alloca double, i32 1
  store double undef, double* %D3_Var
  %F4_Var = alloca float, i32 1
  store float undef, float* %F4_Var
  %D4_Var = alloca double, i32 1
  store double undef, double* %D4_Var
  %F5_Var = alloca float, i32 1
  store float undef, float* %F5_Var
  %D5_Var = alloca double, i32 1
  store double undef, double* %D5_Var
  %F6_Var = alloca float, i32 1
  store float undef, float* %F6_Var
  %D6_Var = alloca double, i32 1
  store double undef, double* %D6_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %ludMm = alloca i64, i32 1
  %ludMn = alloca i64, i32 1
  %ludMj = alloca i64, i32 1
  %lsbaV = alloca i64, i32 1
  %ludMk = alloca i64, i32 1
  %Base_Var = alloca i64*, i32 1
  store i64* %Base_Arg, i64** %Base_Var
  %ludMl = alloca i64, i32 1
  %SpLim_Var = alloca i64, i32 1
  store i64 %SpLim_Arg, i64* %SpLim_Var
  %ludMo = alloca i64, i32 1
  %ludMp = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lndSs = load i64*, i64** %Hp_Var
  %lndSt = getelementptr inbounds i64, i64* %lndSs, i32 6
  %lndSu = ptrtoint i64* %lndSt to i64
  %lndSv = inttoptr i64 %lndSu to i64*
  store i64* %lndSv, i64** %Hp_Var
  %lndSw = load i64*, i64** %Hp_Var
  %lndSx = ptrtoint i64* %lndSw to i64
  %lndSy = load i64*, i64** %Base_Var
  %lndSz = getelementptr inbounds i64, i64* %lndSy, i32 107
  %lndSA = bitcast i64* %lndSz to i64*
  %lndSB = load i64, i64* %lndSA, !tbaa !4
  %lndSC = icmp ugt i64 %lndSx, %lndSB
  br i1 %lndSC, label %cdLD, label %cdLC
cdLC:
  %lndSF = load i64, i64* %R1_Var
  %lndSG = add i64 %lndSF, 7
  %lndSH = inttoptr i64 %lndSG to i64*
  %lndSI = load i64, i64* %lndSH, !tbaa !3
  store i64 %lndSI, i64* %lsbaN
  %lndSL = load i64, i64* %R1_Var
  %lndSM = add i64 %lndSL, 15
  %lndSN = inttoptr i64 %lndSM to i64*
  %lndSO = load i64, i64* %lndSN, !tbaa !3
  store i64 %lndSO, i64* %lsbaO
  %lndSQ = ptrtoint i8* @ghczmprim_GHCziTypes_ZC_con_info to i64
  %lndSP = load i64*, i64** %Hp_Var
  %lndSR = getelementptr inbounds i64, i64* %lndSP, i32 -5
  store i64 %lndSQ, i64* %lndSR, !tbaa !2
  %lndST = load i64, i64* %lsbaN
  %lndSS = load i64*, i64** %Hp_Var
  %lndSU = getelementptr inbounds i64, i64* %lndSS, i32 -4
  store i64 %lndST, i64* %lndSU, !tbaa !2
  %lndSW = load i64, i64* %lsbaO
  %lndSV = load i64*, i64** %Hp_Var
  %lndSX = getelementptr inbounds i64, i64* %lndSV, i32 -3
  store i64 %lndSW, i64* %lndSX, !tbaa !2
  %lndSZ = ptrtoint i8* @Disk_FL_con_info to i64
  %lndSY = load i64*, i64** %Hp_Var
  %lndT0 = getelementptr inbounds i64, i64* %lndSY, i32 -2
  store i64 %lndSZ, i64* %lndT0, !tbaa !2
  %lndT2 = ptrtoint i8* @ghczmprim_GHCziTypes_ZMZN_closure to i64
  %lndT3 = add i64 %lndT2, 1
  %lndT1 = load i64*, i64** %Hp_Var
  %lndT4 = getelementptr inbounds i64, i64* %lndT1, i32 -1
  store i64 %lndT3, i64* %lndT4, !tbaa !2
  %lndT7 = load i64*, i64** %Hp_Var
  %lndT8 = ptrtoint i64* %lndT7 to i64
  %lndT9 = add i64 %lndT8, -38
  %lndT5 = load i64*, i64** %Hp_Var
  %lndTa = getelementptr inbounds i64, i64* %lndT5, i32 0
  store i64 %lndT9, i64* %lndTa, !tbaa !2
  %lndTb = load i64*, i64** %Sp_Var
  %lndTc = getelementptr inbounds i64, i64* %lndTb, i32 1
  %lndTd = bitcast i64* %lndTc to i64*
  %lndTe = load i64, i64* %lndTd, !tbaa !1
  store i64 %lndTe, i64* %lsbaI
  %lndTf = load i64, i64* %lsbaI
  %lndTg = add i64 %lndTf, 8
  %lndTi = load i64*, i64** %Hp_Var
  %lndTj = ptrtoint i64* %lndTi to i64
  %lndTk = add i64 %lndTj, -15
  %lndTl = inttoptr i64 %lndTg to i64*
  store i64 %lndTk, i64* %lndTl, !tbaa !5
  %lndTm = load i64*, i64** %Sp_Var
  %lndTn = getelementptr inbounds i64, i64* %lndTm, i32 2
  %lndTo = bitcast i64* %lndTn to i64*
  %lndTp = load i64, i64* %lndTo, !tbaa !1
  store i64 %lndTp, i64* %lsbaj
  %lndTq = load i64*, i64** %Base_Var
  %lndTr = ptrtoint i64* %lndTq to i64
  %lndTs = inttoptr i64 %lndTr to i8*
  %lndTt = load i64, i64* %lsbaI
  %lndTu = inttoptr i64 %lndTt to i8*
  %lndTv = bitcast i8* @dirty_MUT_VAR to void (i8*, i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  call ccc void (i8*, i8*) %lndTv( i8* %lndTs, i8* %lndTu ) nounwind
  %lndTx = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdLN_info$def to i64
  %lndTw = load i64*, i64** %Sp_Var
  %lndTy = getelementptr inbounds i64, i64* %lndTw, i32 2
  store i64 %lndTx, i64* %lndTy, !tbaa !1
  %lndTz = load i64*, i64** %Sp_Var
  %lndTA = getelementptr inbounds i64, i64* %lndTz, i32 2
  %lndTB = ptrtoint i64* %lndTA to i64
  %lndTC = inttoptr i64 %lndTB to i64*
  store i64* %lndTC, i64** %Sp_Var
  %lndTD = load i64*, i64** %Base_Var
  %lndTE = getelementptr inbounds i64, i64* %lndTD, i32 109
  %lndTF = bitcast i64* %lndTE to i64*
  %lndTG = load i64, i64* %lndTF, !tbaa !4
  store i64 %lndTG, i64* %ludMm
  %lndTH = load i64, i64* %ludMm
  %lndTI = add i64 %lndTH, 24
  %lndTJ = inttoptr i64 %lndTI to i64*
  %lndTK = load i64, i64* %lndTJ, !tbaa !5
  %lndTL = add i64 %lndTK, 16
  %lndTM = load i64*, i64** %Sp_Var
  %lndTN = ptrtoint i64* %lndTM to i64
  %lndTO = inttoptr i64 %lndTL to i64*
  store i64 %lndTN, i64* %lndTO, !tbaa !5
  %lndTP = load i64*, i64** %Base_Var
  %lndTQ = getelementptr inbounds i64, i64* %lndTP, i32 111
  %lndTR = bitcast i64* %lndTQ to i64*
  %lndTS = load i64, i64* %lndTR, !tbaa !4
  store i64 %lndTS, i64* %ludMn
  %lndTT = load i64, i64* %ludMn
  %lndTU = add i64 %lndTT, 8
  %lndTV = load i64*, i64** %Hp_Var
  %lndTW = getelementptr inbounds i64, i64* %lndTV, i32 1
  %lndTX = ptrtoint i64* %lndTW to i64
  %lndTY = inttoptr i64 %lndTU to i64*
  store i64 %lndTX, i64* %lndTY, !tbaa !5
  %lndTZ = load i64, i64* %ludMm
  %lndU0 = add i64 %lndTZ, 104
  %lndU1 = load i64, i64* %ludMm
  %lndU2 = add i64 %lndU1, 104
  %lndU3 = inttoptr i64 %lndU2 to i64*
  %lndU4 = load i64, i64* %lndU3, !tbaa !5
  %lndU5 = load i64*, i64** %Hp_Var
  %lndU6 = getelementptr inbounds i64, i64* %lndU5, i32 1
  %lndU7 = ptrtoint i64* %lndU6 to i64
  %lndU8 = load i64, i64* %ludMn
  %lndU9 = inttoptr i64 %lndU8 to i64*
  %lndUa = load i64, i64* %lndU9, !tbaa !5
  %lndUb = sub i64 %lndU7, %lndUa
  %lndUc = sub i64 %lndU4, %lndUb
  %lndUd = inttoptr i64 %lndU0 to i64*
  store i64 %lndUc, i64* %lndUd, !tbaa !5
  %lndUe = load i64*, i64** %Base_Var
  %lndUf = ptrtoint i64* %lndUe to i64
  %lndUg = inttoptr i64 %lndUf to i8*
  %lndUh = bitcast i8* @suspendThread to i8* (i8*, i64)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lndUi = call ccc i8* (i8*, i64) %lndUh( i8* %lndUg, i64 0 ) nounwind
  %lndUj = ptrtoint i8* %lndUi to i64
  store i64 %lndUj, i64* %ludMj
  %lndUk = load i64, i64* %lsbaj
  %lndUl = bitcast i8* @ghczuwrapperZC1ZCunixzm2zi7zi2zi0ZCSystemziPosixziUnistdZCfsync to i64 (i64)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lndUm = call ccc i64 (i64) %lndUl( i64 %lndUk ) nounwind
  store i64 %lndUm, i64* %lsbaV
  %lndUn = load i64, i64* %ludMj
  %lndUo = inttoptr i64 %lndUn to i8*
  %lndUp = bitcast i8* @resumeThread to i8* (i8*)*
  store i64 undef, i64* %R3_Var
  store i64 undef, i64* %R4_Var
  store i64 undef, i64* %R5_Var
  store i64 undef, i64* %R6_Var
  store float undef, float* %F1_Var
  store double undef, double* %D1_Var
  store float undef, float* %F2_Var
  store double undef, double* %D2_Var
  store float undef, float* %F3_Var
  store double undef, double* %D3_Var
  store float undef, float* %F4_Var
  store double undef, double* %D4_Var
  store float undef, float* %F5_Var
  store double undef, double* %D5_Var
  store float undef, float* %F6_Var
  store double undef, double* %D6_Var
  %lndUq = call ccc i8* (i8*) %lndUp( i8* %lndUo ) nounwind
  %lndUr = ptrtoint i8* %lndUq to i64
  store i64 %lndUr, i64* %ludMk
  %lndUs = load i64, i64* %ludMk
  %lndUt = inttoptr i64 %lndUs to i64*
  store i64* %lndUt, i64** %Base_Var
  %lndUu = load i64*, i64** %Base_Var
  %lndUv = getelementptr inbounds i64, i64* %lndUu, i32 109
  %lndUw = bitcast i64* %lndUv to i64*
  %lndUx = load i64, i64* %lndUw, !tbaa !4
  store i64 %lndUx, i64* %ludMm
  %lndUy = load i64, i64* %ludMm
  %lndUz = add i64 %lndUy, 24
  %lndUA = inttoptr i64 %lndUz to i64*
  %lndUB = load i64, i64* %lndUA, !tbaa !5
  store i64 %lndUB, i64* %ludMl
  %lndUC = load i64, i64* %ludMl
  %lndUD = add i64 %lndUC, 16
  %lndUE = inttoptr i64 %lndUD to i64*
  %lndUF = load i64, i64* %lndUE, !tbaa !5
  %lndUG = inttoptr i64 %lndUF to i64*
  store i64* %lndUG, i64** %Sp_Var
  %lndUH = load i64, i64* %ludMl
  %lndUI = add i64 %lndUH, 192
  store i64 %lndUI, i64* %SpLim_Var
  %lndUJ = load i64*, i64** %Base_Var
  %lndUK = getelementptr inbounds i64, i64* %lndUJ, i32 113
  store i64 0, i64* %lndUK, !tbaa !4
  %lndUL = load i64*, i64** %Base_Var
  %lndUM = getelementptr inbounds i64, i64* %lndUL, i32 111
  %lndUN = bitcast i64* %lndUM to i64*
  %lndUO = load i64, i64* %lndUN, !tbaa !4
  store i64 %lndUO, i64* %ludMn
  %lndUP = load i64, i64* %ludMn
  %lndUQ = add i64 %lndUP, 8
  %lndUR = inttoptr i64 %lndUQ to i64*
  %lndUS = load i64, i64* %lndUR, !tbaa !5
  store i64 %lndUS, i64* %ludMo
  %lndUT = load i64, i64* %ludMo
  %lndUU = add i64 %lndUT, -8
  %lndUV = inttoptr i64 %lndUU to i64*
  store i64* %lndUV, i64** %Hp_Var
  %lndUW = load i64, i64* %ludMn
  %lndUX = inttoptr i64 %lndUW to i64*
  %lndUY = load i64, i64* %lndUX, !tbaa !5
  store i64 %lndUY, i64* %ludMp
  %lndV0 = load i64, i64* %ludMp
  %lndV1 = load i64, i64* %ludMn
  %lndV2 = add i64 %lndV1, 48
  %lndV3 = inttoptr i64 %lndV2 to i32*
  %lndV4 = load i32, i32* %lndV3, !tbaa !5
  %lndV5 = sext i32 %lndV4 to i64
  %lndV6 = shl i64 %lndV5, 12
  %lndV7 = add i64 %lndV6, -1
  %lndV8 = add i64 %lndV0, %lndV7
  %lndUZ = load i64*, i64** %Base_Var
  %lndV9 = getelementptr inbounds i64, i64* %lndUZ, i32 107
  store i64 %lndV8, i64* %lndV9, !tbaa !4
  %lndVa = load i64, i64* %ludMm
  %lndVb = add i64 %lndVa, 104
  %lndVc = load i64, i64* %ludMm
  %lndVd = add i64 %lndVc, 104
  %lndVe = inttoptr i64 %lndVd to i64*
  %lndVf = load i64, i64* %lndVe, !tbaa !5
  %lndVg = load i64, i64* %ludMo
  %lndVh = load i64, i64* %ludMp
  %lndVi = sub i64 %lndVg, %lndVh
  %lndVj = add i64 %lndVf, %lndVi
  %lndVk = inttoptr i64 %lndVb to i64*
  store i64 %lndVj, i64* %lndVk, !tbaa !5
  %lndVl = load i64, i64* %lsbaV
  store i64 %lndVl, i64* %R1_Var
  %lndVm = load i64*, i64** %Sp_Var
  %lndVn = getelementptr inbounds i64, i64* %lndVm, i32 0
  %lndVo = bitcast i64* %lndVn to i64*
  %lndVp = load i64, i64* %lndVo, !tbaa !1
  %lndVq = inttoptr i64 %lndVp to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndVr = load i64*, i64** %Base_Var
  %lndVs = load i64*, i64** %Sp_Var
  %lndVt = load i64*, i64** %Hp_Var
  %lndVu = load i64, i64* %R1_Var
  %lndVv = load i64, i64* %SpLim_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndVq( i64* %lndVr, i64* %lndVs, i64* %lndVt, i64 %lndVu, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %lndVv ) nounwind
  ret void
cdLD:
  %lndVw = load i64*, i64** %Base_Var
  %lndVx = getelementptr inbounds i64, i64* %lndVw, i32 113
  store i64 48, i64* %lndVx, !tbaa !4
  %lndVy = load i64, i64* %R1_Var
  store i64 %lndVy, i64* %R1_Var
  %lndVz = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndVA = load i64*, i64** %Base_Var
  %lndVB = load i64*, i64** %Sp_Var
  %lndVC = load i64*, i64** %Hp_Var
  %lndVD = load i64, i64* %R1_Var
  %lndVE = load i64, i64* %SpLim_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndVz( i64* %lndVA, i64* %lndVB, i64* %lndVC, i64 %lndVD, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %lndVE ) nounwind
  ret void
}
@cdLN_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdLN_info$def to i8*)
define internal ghccc void @cdLN_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdLN_info$def to i64)),i64 176), i64 0, i64 1152921508901814304}>
{
cdLN:
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %lndVF = trunc i64 %R1_Arg to i32
  %lndVG = sext i32 %lndVF to i64
  switch i64 %lndVG, label %udM9 [i64 -1, label %cdM4]
udM9:
  %lndVH = load i64*, i64** %Sp_Var
  %lndVI = getelementptr inbounds i64, i64* %lndVH, i32 1
  %lndVJ = ptrtoint i64* %lndVI to i64
  %lndVK = inttoptr i64 %lndVJ to i64*
  store i64* %lndVK, i64** %Sp_Var
  %lndVL = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdLV_entry$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndVM = load i64*, i64** %Sp_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndVL( i64* %Base_Arg, i64* %lndVM, i64* %Hp_Arg, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdM4:
  %lndVO = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdM3_info$def to i64
  %lndVN = load i64*, i64** %Sp_Var
  %lndVP = getelementptr inbounds i64, i64* %lndVN, i32 0
  store i64 %lndVO, i64* %lndVP, !tbaa !1
  %lndVQ = ptrtoint i8* @unixzm2zi7zi2zi0_SystemziPosixziUnistd_fileSynchronise2_closure to i64
  store i64 %lndVQ, i64* %R2_Var
  %lndVR = bitcast i8* @base_ForeignziCziError_throwErrno1_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndVS = load i64*, i64** %Sp_Var
  %lndVT = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndVR( i64* %Base_Arg, i64* %lndVS, i64* %Hp_Arg, i64 %R1_Arg, i64 %lndVT, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdM3_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdM3_info$def to i8*)
define internal ghccc void @cdM3_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
cdM3:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lndVU = ptrtoint i8* @ghczmprim_GHCziTuple_Z0T_closure to i64
  %lndVV = add i64 %lndVU, 1
  store i64 %lndVV, i64* %R1_Var
  %lndVW = load i64*, i64** %Sp_Var
  %lndVX = getelementptr inbounds i64, i64* %lndVW, i32 1
  %lndVY = ptrtoint i64* %lndVX to i64
  %lndVZ = inttoptr i64 %lndVY to i64*
  store i64* %lndVZ, i64** %Sp_Var
  %lndW0 = load i64*, i64** %Sp_Var
  %lndW1 = getelementptr inbounds i64, i64* %lndW0, i32 0
  %lndW2 = bitcast i64* %lndW1 to i64*
  %lndW3 = load i64, i64* %lndW2, !tbaa !1
  %lndW4 = inttoptr i64 %lndW3 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndW5 = load i64*, i64** %Sp_Var
  %lndW6 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndW4( i64* %Base_Arg, i64* %lndW5, i64* %Hp_Arg, i64 %lndW6, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdL1_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdL1_info$def to i8*)
define internal ghccc void @cdL1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdL1_info$def to i64)),i64 176), i64 0, i64 1152921508901814304}>
{
cdL1:
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %lndW7 = trunc i64 %R1_Arg to i32
  %lndW8 = sext i32 %lndW7 to i64
  switch i64 %lndW8, label %udMa [i64 -1, label %cdLi]
udMa:
  %lndW9 = load i64*, i64** %Sp_Var
  %lndWa = getelementptr inbounds i64, i64* %lndW9, i32 1
  %lndWb = ptrtoint i64* %lndWa to i64
  %lndWc = inttoptr i64 %lndWb to i64*
  store i64* %lndWc, i64** %Sp_Var
  %lndWd = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdLV_entry$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndWe = load i64*, i64** %Sp_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndWd( i64* %Base_Arg, i64* %lndWe, i64* %Hp_Arg, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdLi:
  %lndWg = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdLh_info$def to i64
  %lndWf = load i64*, i64** %Sp_Var
  %lndWh = getelementptr inbounds i64, i64* %lndWf, i32 0
  store i64 %lndWg, i64* %lndWh, !tbaa !1
  %lndWi = ptrtoint i8* @unixzm2zi7zi2zi0_SystemziPosixziUnistd_fileSynchronise2_closure to i64
  store i64 %lndWi, i64* %R2_Var
  %lndWj = bitcast i8* @base_ForeignziCziError_throwErrno1_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndWk = load i64*, i64** %Sp_Var
  %lndWl = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndWj( i64* %Base_Arg, i64* %lndWk, i64* %Hp_Arg, i64 %R1_Arg, i64 %lndWl, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdLh_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdLh_info$def to i8*)
define internal ghccc void @cdLh_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
cdLh:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lndWm = ptrtoint i8* @ghczmprim_GHCziTuple_Z0T_closure to i64
  %lndWn = add i64 %lndWm, 1
  store i64 %lndWn, i64* %R1_Var
  %lndWo = load i64*, i64** %Sp_Var
  %lndWp = getelementptr inbounds i64, i64* %lndWo, i32 1
  %lndWq = ptrtoint i64* %lndWp to i64
  %lndWr = inttoptr i64 %lndWq to i64*
  store i64* %lndWr, i64** %Sp_Var
  %lndWs = load i64*, i64** %Sp_Var
  %lndWt = getelementptr inbounds i64, i64* %lndWs, i32 0
  %lndWu = bitcast i64* %lndWt to i64*
  %lndWv = load i64, i64* %lndWu, !tbaa !1
  %lndWw = inttoptr i64 %lndWv to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndWx = load i64*, i64** %Sp_Var
  %lndWy = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndWw( i64* %Base_Arg, i64* %lndWx, i64* %Hp_Arg, i64 %lndWy, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@cdLV_entry = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @cdLV_entry$def to i8*)
define internal ghccc void @cdLV_entry$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind
{
cdLV:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lndWz = ptrtoint i8* @ghczmprim_GHCziTuple_Z0T_closure to i64
  %lndWA = add i64 %lndWz, 1
  store i64 %lndWA, i64* %R1_Var
  %lndWB = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  %lndWC = bitcast i64* %lndWB to i64*
  %lndWD = load i64, i64* %lndWC, !tbaa !1
  %lndWE = inttoptr i64 %lndWD to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndWF = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndWE( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lndWF, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_synczudisk_closure_struct = type <{i64, i64}>
@Disk_synczudisk_closure$def = internal global %Disk_synczudisk_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_synczudisk_info$def to i64), i64 0}>
@Disk_synczudisk_closure = alias i8* bitcast (%Disk_synczudisk_closure_struct* @Disk_synczudisk_closure$def to i8*)
@Disk_synczudisk_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_synczudisk_info$def to i8*)
define ghccc void @Disk_synczudisk_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @Sbl8_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_synczudisk_info$def to i64)),i64 408), i64 8589934597, i64 0, i64 4294967311}>
{
cdWL:
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lndWO = load i64, i64* %R2_Var
  store i64 %lndWO, i64* %R2_Var
  %lndWP = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_synczudisk1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndWQ = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndWP( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lndWQ, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_trimzudisk1_closure_struct = type <{i64}>
@Disk_trimzudisk1_closure$def = internal global %Disk_trimzudisk1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_trimzudisk1_info$def to i64)}>
@Disk_trimzudisk1_closure = alias i8* bitcast (%Disk_trimzudisk1_closure_struct* @Disk_trimzudisk1_closure$def to i8*)
@Disk_trimzudisk1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_trimzudisk1_info$def to i8*)
define ghccc void @Disk_trimzudisk1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 12884901903, i64 0, i64 15}>
{
cdWW:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lndWZ = ptrtoint i8* @ghczmprim_GHCziTuple_Z0T_closure to i64
  %lndX0 = add i64 %lndWZ, 1
  store i64 %lndX0, i64* %R1_Var
  %lndX1 = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  %lndX2 = bitcast i64* %lndX1 to i64*
  %lndX3 = load i64, i64* %lndX2, !tbaa !1
  %lndX4 = inttoptr i64 %lndX3 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndX5 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndX4( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lndX5, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_trimzudisk_closure_struct = type <{i64}>
@Disk_trimzudisk_closure$def = internal global %Disk_trimzudisk_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_trimzudisk_info$def to i64)}>
@Disk_trimzudisk_closure = alias i8* bitcast (%Disk_trimzudisk_closure_struct* @Disk_trimzudisk_closure$def to i8*)
@Disk_trimzudisk_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_trimzudisk_info$def to i8*)
define ghccc void @Disk_trimzudisk_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 12884901903, i64 0, i64 15}>
{
cdXb:
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lndXe = load i64, i64* %R3_Var
  store i64 %lndXe, i64* %R3_Var
  %lndXf = load i64, i64* %R2_Var
  store i64 %lndXf, i64* %R2_Var
  %lndXg = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_trimzudisk1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndXh = load i64, i64* %R2_Var
  %lndXi = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndXg( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lndXh, i64 %lndXi, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_VS_closure_struct = type <{i64}>
@Disk_VS_closure$def = internal global %Disk_VS_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_VS_info$def to i64)}>
@Disk_VS_closure = alias i8* bitcast (%Disk_VS_closure_struct* @Disk_VS_closure$def to i8*)
@Disk_VS_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_VS_info$def to i8*)
define internal ghccc void @Disk_VS_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934607, i64 0, i64 15}>
{
cdXp:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lndXu = load i64*, i64** %Hp_Var
  %lndXv = getelementptr inbounds i64, i64* %lndXu, i32 3
  %lndXw = ptrtoint i64* %lndXv to i64
  %lndXx = inttoptr i64 %lndXw to i64*
  store i64* %lndXx, i64** %Hp_Var
  %lndXy = load i64*, i64** %Hp_Var
  %lndXz = ptrtoint i64* %lndXy to i64
  %lndXA = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lndXB = bitcast i64* %lndXA to i64*
  %lndXC = load i64, i64* %lndXB, !tbaa !4
  %lndXD = icmp ugt i64 %lndXz, %lndXC
  br i1 %lndXD, label %cdXt, label %cdXs
cdXs:
  %lndXF = ptrtoint i8* @Disk_VS_con_info to i64
  %lndXE = load i64*, i64** %Hp_Var
  %lndXG = getelementptr inbounds i64, i64* %lndXE, i32 -2
  store i64 %lndXF, i64* %lndXG, !tbaa !2
  %lndXI = load i64, i64* %R2_Var
  %lndXH = load i64*, i64** %Hp_Var
  %lndXJ = getelementptr inbounds i64, i64* %lndXH, i32 -1
  store i64 %lndXI, i64* %lndXJ, !tbaa !2
  %lndXL = load i64, i64* %R3_Var
  %lndXK = load i64*, i64** %Hp_Var
  %lndXM = getelementptr inbounds i64, i64* %lndXK, i32 0
  store i64 %lndXL, i64* %lndXM, !tbaa !2
  %lndXO = load i64*, i64** %Hp_Var
  %lndXP = ptrtoint i64* %lndXO to i64
  %lndXQ = add i64 %lndXP, -15
  store i64 %lndXQ, i64* %R1_Var
  %lndXR = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  %lndXS = bitcast i64* %lndXR to i64*
  %lndXT = load i64, i64* %lndXS, !tbaa !1
  %lndXU = inttoptr i64 %lndXT to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndXV = load i64*, i64** %Hp_Var
  %lndXW = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndXU( i64* %Base_Arg, i64* %Sp_Arg, i64* %lndXV, i64 %lndXW, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdXt:
  %lndXX = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lndXX, !tbaa !4
  %lndXY = load i64, i64* %R3_Var
  store i64 %lndXY, i64* %R3_Var
  %lndXZ = load i64, i64* %R2_Var
  store i64 %lndXZ, i64* %R2_Var
  %lndY0 = ptrtoint %Disk_VS_closure_struct* @Disk_VS_closure$def to i64
  store i64 %lndY0, i64* %R1_Var
  %lndY1 = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lndY2 = bitcast i64* %lndY1 to i64*
  %lndY3 = load i64, i64* %lndY2, !tbaa !4
  %lndY4 = inttoptr i64 %lndY3 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndY5 = load i64*, i64** %Hp_Var
  %lndY6 = load i64, i64* %R1_Var
  %lndY7 = load i64, i64* %R2_Var
  %lndY8 = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndY4( i64* %Base_Arg, i64* %Sp_Arg, i64* %lndY5, i64 %lndY6, i64 %lndY7, i64 %lndY8, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_FL_closure_struct = type <{i64}>
@Disk_FL_closure$def = internal global %Disk_FL_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_FL_info$def to i64)}>
@Disk_FL_closure = alias i8* bitcast (%Disk_FL_closure_struct* @Disk_FL_closure$def to i8*)
@Disk_FL_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_FL_info$def to i8*)
define internal ghccc void @Disk_FL_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934607, i64 0, i64 15}>
{
cdYf:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lndYk = load i64*, i64** %Hp_Var
  %lndYl = getelementptr inbounds i64, i64* %lndYk, i32 3
  %lndYm = ptrtoint i64* %lndYl to i64
  %lndYn = inttoptr i64 %lndYm to i64*
  store i64* %lndYn, i64** %Hp_Var
  %lndYo = load i64*, i64** %Hp_Var
  %lndYp = ptrtoint i64* %lndYo to i64
  %lndYq = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lndYr = bitcast i64* %lndYq to i64*
  %lndYs = load i64, i64* %lndYr, !tbaa !4
  %lndYt = icmp ugt i64 %lndYp, %lndYs
  br i1 %lndYt, label %cdYj, label %cdYi
cdYi:
  %lndYv = ptrtoint i8* @Disk_FL_con_info to i64
  %lndYu = load i64*, i64** %Hp_Var
  %lndYw = getelementptr inbounds i64, i64* %lndYu, i32 -2
  store i64 %lndYv, i64* %lndYw, !tbaa !2
  %lndYy = load i64, i64* %R2_Var
  %lndYx = load i64*, i64** %Hp_Var
  %lndYz = getelementptr inbounds i64, i64* %lndYx, i32 -1
  store i64 %lndYy, i64* %lndYz, !tbaa !2
  %lndYB = load i64, i64* %R3_Var
  %lndYA = load i64*, i64** %Hp_Var
  %lndYC = getelementptr inbounds i64, i64* %lndYA, i32 0
  store i64 %lndYB, i64* %lndYC, !tbaa !2
  %lndYE = load i64*, i64** %Hp_Var
  %lndYF = ptrtoint i64* %lndYE to i64
  %lndYG = add i64 %lndYF, -15
  store i64 %lndYG, i64* %R1_Var
  %lndYH = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  %lndYI = bitcast i64* %lndYH to i64*
  %lndYJ = load i64, i64* %lndYI, !tbaa !1
  %lndYK = inttoptr i64 %lndYJ to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndYL = load i64*, i64** %Hp_Var
  %lndYM = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndYK( i64* %Base_Arg, i64* %Sp_Arg, i64* %lndYL, i64 %lndYM, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdYj:
  %lndYN = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lndYN, !tbaa !4
  %lndYO = load i64, i64* %R3_Var
  store i64 %lndYO, i64* %R3_Var
  %lndYP = load i64, i64* %R2_Var
  store i64 %lndYP, i64* %R2_Var
  %lndYQ = ptrtoint %Disk_FL_closure_struct* @Disk_FL_closure$def to i64
  store i64 %lndYQ, i64* %R1_Var
  %lndYR = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lndYS = bitcast i64* %lndYR to i64*
  %lndYT = load i64, i64* %lndYS, !tbaa !4
  %lndYU = inttoptr i64 %lndYT to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndYV = load i64*, i64** %Hp_Var
  %lndYW = load i64, i64* %R1_Var
  %lndYX = load i64, i64* %R2_Var
  %lndYY = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndYU( i64* %Base_Arg, i64* %Sp_Arg, i64* %lndYV, i64 %lndYW, i64 %lndYX, i64 %lndYY, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_Stats_closure_struct = type <{i64}>
@Disk_Stats_closure$def = internal global %Disk_Stats_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_Stats_info$def to i64)}>
@Disk_Stats_closure = alias i8* bitcast (%Disk_Stats_closure_struct* @Disk_Stats_closure$def to i8*)
@Disk_Stats_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_Stats_info$def to i8*)
define internal ghccc void @Disk_Stats_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 12884901904, i64 0, i64 15}>
{
cdZ5:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %R4_Var = alloca i64, i32 1
  store i64 %R4_Arg, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %lndZa = load i64*, i64** %Hp_Var
  %lndZb = getelementptr inbounds i64, i64* %lndZa, i32 4
  %lndZc = ptrtoint i64* %lndZb to i64
  %lndZd = inttoptr i64 %lndZc to i64*
  store i64* %lndZd, i64** %Hp_Var
  %lndZe = load i64*, i64** %Hp_Var
  %lndZf = ptrtoint i64* %lndZe to i64
  %lndZg = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lndZh = bitcast i64* %lndZg to i64*
  %lndZi = load i64, i64* %lndZh, !tbaa !4
  %lndZj = icmp ugt i64 %lndZf, %lndZi
  br i1 %lndZj, label %cdZ9, label %cdZ8
cdZ8:
  %lndZl = ptrtoint i8* @Disk_Stats_con_info to i64
  %lndZk = load i64*, i64** %Hp_Var
  %lndZm = getelementptr inbounds i64, i64* %lndZk, i32 -3
  store i64 %lndZl, i64* %lndZm, !tbaa !2
  %lndZo = load i64, i64* %R2_Var
  %lndZn = load i64*, i64** %Hp_Var
  %lndZp = getelementptr inbounds i64, i64* %lndZn, i32 -2
  store i64 %lndZo, i64* %lndZp, !tbaa !2
  %lndZr = load i64, i64* %R3_Var
  %lndZq = load i64*, i64** %Hp_Var
  %lndZs = getelementptr inbounds i64, i64* %lndZq, i32 -1
  store i64 %lndZr, i64* %lndZs, !tbaa !2
  %lndZu = load i64, i64* %R4_Var
  %lndZt = load i64*, i64** %Hp_Var
  %lndZv = getelementptr inbounds i64, i64* %lndZt, i32 0
  store i64 %lndZu, i64* %lndZv, !tbaa !2
  %lndZx = load i64*, i64** %Hp_Var
  %lndZy = ptrtoint i64* %lndZx to i64
  %lndZz = add i64 %lndZy, -23
  store i64 %lndZz, i64* %R1_Var
  %lndZA = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  %lndZB = bitcast i64* %lndZA to i64*
  %lndZC = load i64, i64* %lndZB, !tbaa !1
  %lndZD = inttoptr i64 %lndZC to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndZE = load i64*, i64** %Hp_Var
  %lndZF = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndZD( i64* %Base_Arg, i64* %Sp_Arg, i64* %lndZE, i64 %lndZF, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
cdZ9:
  %lndZG = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 32, i64* %lndZG, !tbaa !4
  %lndZH = load i64, i64* %R4_Var
  store i64 %lndZH, i64* %R4_Var
  %lndZI = load i64, i64* %R3_Var
  store i64 %lndZI, i64* %R3_Var
  %lndZJ = load i64, i64* %R2_Var
  store i64 %lndZJ, i64* %R2_Var
  %lndZK = ptrtoint %Disk_Stats_closure_struct* @Disk_Stats_closure$def to i64
  store i64 %lndZK, i64* %R1_Var
  %lndZL = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lndZM = bitcast i64* %lndZL to i64*
  %lndZN = load i64, i64* %lndZM, !tbaa !4
  %lndZO = inttoptr i64 %lndZN to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndZP = load i64*, i64** %Hp_Var
  %lndZQ = load i64, i64* %R1_Var
  %lndZR = load i64, i64* %R2_Var
  %lndZS = load i64, i64* %R3_Var
  %lndZT = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndZO( i64* %Base_Arg, i64* %Sp_Arg, i64* %lndZP, i64 %lndZQ, i64 %lndZR, i64 %lndZS, i64 %lndZT, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Disk_S_closure_struct = type <{i64}>
@Disk_S_closure$def = internal global %Disk_S_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_S_info$def to i64)}>
@Disk_S_closure = alias i8* bitcast (%Disk_S_closure_struct* @Disk_S_closure$def to i8*)
@Disk_S_slow = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_S_slow$def to i8*)
define ghccc void @Disk_S_slow$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind
{
cdZW:
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lne06 = load i64*, i64** %Sp_Var
  %lne07 = getelementptr inbounds i64, i64* %lne06, i32 3
  %lne08 = bitcast i64* %lne07 to i64*
  %lne09 = load i64, i64* %lne08, !tbaa !1
  store i64 %lne09, i64* %R5_Var
  %lne0a = load i64*, i64** %Sp_Var
  %lne0b = getelementptr inbounds i64, i64* %lne0a, i32 2
  %lne0c = bitcast i64* %lne0b to i64*
  %lne0d = load i64, i64* %lne0c, !tbaa !1
  store i64 %lne0d, i64* %R4_Var
  %lne0e = load i64*, i64** %Sp_Var
  %lne0f = getelementptr inbounds i64, i64* %lne0e, i32 1
  %lne0g = bitcast i64* %lne0f to i64*
  %lne0h = load i64, i64* %lne0g, !tbaa !1
  store i64 %lne0h, i64* %R3_Var
  %lne0i = load i64*, i64** %Sp_Var
  %lne0j = getelementptr inbounds i64, i64* %lne0i, i32 0
  %lne0k = bitcast i64* %lne0j to i64*
  %lne0l = load i64, i64* %lne0k, !tbaa !1
  store i64 %lne0l, i64* %R2_Var
  %lne0m = load i64, i64* %R1_Var
  store i64 %lne0m, i64* %R1_Var
  %lne0n = load i64*, i64** %Sp_Var
  %lne0o = getelementptr inbounds i64, i64* %lne0n, i32 4
  %lne0p = ptrtoint i64* %lne0o to i64
  %lne0q = inttoptr i64 %lne0p to i64*
  store i64* %lne0q, i64** %Sp_Var
  %lne0r = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_S_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lne0s = load i64*, i64** %Sp_Var
  %lne0t = load i64, i64* %R1_Var
  %lne0u = load i64, i64* %R2_Var
  %lne0v = load i64, i64* %R3_Var
  %lne0w = load i64, i64* %R4_Var
  %lne0x = load i64, i64* %R5_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lne0r( i64* %Base_Arg, i64* %lne0s, i64* %Hp_Arg, i64 %lne0t, i64 %lne0u, i64 %lne0v, i64 %lne0w, i64 %lne0x, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@Disk_S_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_S_info$def to i8*)
define internal ghccc void @Disk_S_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_S_slow$def to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_S_info$def to i64)),i64 0), i64 68, i64 0, i64 17179869184, i64 0, i64 15}>
{
ce01:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lne0y = load i64*, i64** %Hp_Var
  %lne0z = getelementptr inbounds i64, i64* %lne0y, i32 5
  %lne0A = ptrtoint i64* %lne0z to i64
  %lne0B = inttoptr i64 %lne0A to i64*
  store i64* %lne0B, i64** %Hp_Var
  %lne0C = load i64*, i64** %Hp_Var
  %lne0D = ptrtoint i64* %lne0C to i64
  %lne0E = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lne0F = bitcast i64* %lne0E to i64*
  %lne0G = load i64, i64* %lne0F, !tbaa !4
  %lne0H = icmp ugt i64 %lne0D, %lne0G
  br i1 %lne0H, label %ce05, label %ce04
ce04:
  %lne0J = ptrtoint i8* @Disk_S_con_info to i64
  %lne0I = load i64*, i64** %Hp_Var
  %lne0K = getelementptr inbounds i64, i64* %lne0I, i32 -4
  store i64 %lne0J, i64* %lne0K, !tbaa !2
  %lne0L = load i64*, i64** %Hp_Var
  %lne0M = getelementptr inbounds i64, i64* %lne0L, i32 -3
  store i64 %R3_Arg, i64* %lne0M, !tbaa !2
  %lne0N = load i64*, i64** %Hp_Var
  %lne0O = getelementptr inbounds i64, i64* %lne0N, i32 -2
  store i64 %R4_Arg, i64* %lne0O, !tbaa !2
  %lne0P = load i64*, i64** %Hp_Var
  %lne0Q = getelementptr inbounds i64, i64* %lne0P, i32 -1
  store i64 %R5_Arg, i64* %lne0Q, !tbaa !2
  %lne0R = load i64*, i64** %Hp_Var
  %lne0S = getelementptr inbounds i64, i64* %lne0R, i32 0
  store i64 %R2_Arg, i64* %lne0S, !tbaa !2
  %lne0U = load i64*, i64** %Hp_Var
  %lne0V = ptrtoint i64* %lne0U to i64
  %lne0W = add i64 %lne0V, -31
  store i64 %lne0W, i64* %R1_Var
  %lne0X = load i64*, i64** %Sp_Var
  %lne0Y = getelementptr inbounds i64, i64* %lne0X, i32 0
  %lne0Z = bitcast i64* %lne0Y to i64*
  %lne10 = load i64, i64* %lne0Z, !tbaa !1
  %lne11 = inttoptr i64 %lne10 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lne12 = load i64*, i64** %Sp_Var
  %lne13 = load i64*, i64** %Hp_Var
  %lne14 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lne11( i64* %Base_Arg, i64* %lne12, i64* %lne13, i64 %lne14, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ce05:
  %lne15 = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 40, i64* %lne15, !tbaa !4
  %lne16 = ptrtoint %Disk_S_closure_struct* @Disk_S_closure$def to i64
  store i64 %lne16, i64* %R1_Var
  %lne17 = load i64*, i64** %Sp_Var
  %lne18 = getelementptr inbounds i64, i64* %lne17, i32 -4
  store i64 %R2_Arg, i64* %lne18, !tbaa !1
  %lne19 = load i64*, i64** %Sp_Var
  %lne1a = getelementptr inbounds i64, i64* %lne19, i32 -3
  store i64 %R3_Arg, i64* %lne1a, !tbaa !1
  %lne1b = load i64*, i64** %Sp_Var
  %lne1c = getelementptr inbounds i64, i64* %lne1b, i32 -2
  store i64 %R4_Arg, i64* %lne1c, !tbaa !1
  %lne1d = load i64*, i64** %Sp_Var
  %lne1e = getelementptr inbounds i64, i64* %lne1d, i32 -1
  store i64 %R5_Arg, i64* %lne1e, !tbaa !1
  %lne1f = load i64*, i64** %Sp_Var
  %lne1g = getelementptr inbounds i64, i64* %lne1f, i32 -4
  %lne1h = ptrtoint i64* %lne1g to i64
  %lne1i = inttoptr i64 %lne1h to i64*
  store i64* %lne1i, i64** %Sp_Var
  %lne1j = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lne1k = bitcast i64* %lne1j to i64*
  %lne1l = load i64, i64* %lne1k, !tbaa !4
  %lne1m = inttoptr i64 %lne1l to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lne1n = load i64*, i64** %Sp_Var
  %lne1o = load i64*, i64** %Hp_Var
  %lne1p = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lne1m( i64* %Base_Arg, i64* %lne1n, i64* %lne1o, i64 %lne1p, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%ie1t_str_struct = type <{[13 x i8]}>
%ie1u_str_struct = type <{[13 x i8]}>
@ie1t_str$def = internal constant %ie1t_str_struct<{[13 x i8] [i8 109, i8 97, i8 105, i8 110, i8 58, i8 68, i8 105, i8 115, i8 107, i8 46, i8 86, i8 83, i8 0]}>
@ie1t_str = internal alias i8* bitcast (%ie1t_str_struct* @ie1t_str$def to i8*)
@ie1u_str$def = internal constant %ie1u_str_struct<{[13 x i8] [i8 109, i8 97, i8 105, i8 110, i8 58, i8 68, i8 105, i8 115, i8 107, i8 46, i8 86, i8 83, i8 0]}>
@ie1u_str = internal alias i8* bitcast (%ie1u_str_struct* @ie1u_str$def to i8*)
@Disk_VS_con_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_VS_con_info$def to i8*)
define ghccc void @Disk_VS_con_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (%ie1t_str_struct* @ie1t_str$def to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_VS_con_info$def to i64)),i64 0), i64 2, i64 4}>
{
ce1r:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lne1w = load i64, i64* %R1_Var
  %lne1x = add i64 %lne1w, 1
  store i64 %lne1x, i64* %R1_Var
  %lne1y = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  %lne1z = bitcast i64* %lne1y to i64*
  %lne1A = load i64, i64* %lne1z, !tbaa !1
  %lne1B = inttoptr i64 %lne1A to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lne1C = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lne1B( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lne1C, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@Disk_VS_static_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_VS_static_info$def to i8*)
define ghccc void @Disk_VS_static_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (%ie1u_str_struct* @ie1u_str$def to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_VS_static_info$def to i64)),i64 0), i64 2, i64 7}>
{
ce1s:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lne1E = load i64, i64* %R1_Var
  %lne1F = add i64 %lne1E, 1
  store i64 %lne1F, i64* %R1_Var
  %lne1G = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  %lne1H = bitcast i64* %lne1G to i64*
  %lne1I = load i64, i64* %lne1H, !tbaa !1
  %lne1J = inttoptr i64 %lne1I to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lne1K = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lne1J( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lne1K, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%ie1N_str_struct = type <{[13 x i8]}>
%ie1O_str_struct = type <{[13 x i8]}>
@ie1N_str$def = internal constant %ie1N_str_struct<{[13 x i8] [i8 109, i8 97, i8 105, i8 110, i8 58, i8 68, i8 105, i8 115, i8 107, i8 46, i8 70, i8 76, i8 0]}>
@ie1N_str = internal alias i8* bitcast (%ie1N_str_struct* @ie1N_str$def to i8*)
@ie1O_str$def = internal constant %ie1O_str_struct<{[13 x i8] [i8 109, i8 97, i8 105, i8 110, i8 58, i8 68, i8 105, i8 115, i8 107, i8 46, i8 70, i8 76, i8 0]}>
@ie1O_str = internal alias i8* bitcast (%ie1O_str_struct* @ie1O_str$def to i8*)
@Disk_FL_con_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_FL_con_info$def to i8*)
define ghccc void @Disk_FL_con_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (%ie1N_str_struct* @ie1N_str$def to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_FL_con_info$def to i64)),i64 0), i64 2, i64 4}>
{
ce1L:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lne1Q = load i64, i64* %R1_Var
  %lne1R = add i64 %lne1Q, 1
  store i64 %lne1R, i64* %R1_Var
  %lne1S = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  %lne1T = bitcast i64* %lne1S to i64*
  %lne1U = load i64, i64* %lne1T, !tbaa !1
  %lne1V = inttoptr i64 %lne1U to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lne1W = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lne1V( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lne1W, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@Disk_FL_static_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_FL_static_info$def to i8*)
define ghccc void @Disk_FL_static_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (%ie1O_str_struct* @ie1O_str$def to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_FL_static_info$def to i64)),i64 0), i64 2, i64 7}>
{
ce1M:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lne1Y = load i64, i64* %R1_Var
  %lne1Z = add i64 %lne1Y, 1
  store i64 %lne1Z, i64* %R1_Var
  %lne20 = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  %lne21 = bitcast i64* %lne20 to i64*
  %lne22 = load i64, i64* %lne21, !tbaa !1
  %lne23 = inttoptr i64 %lne22 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lne24 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lne23( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lne24, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%ie27_str_struct = type <{[16 x i8]}>
%ie28_str_struct = type <{[16 x i8]}>
@ie27_str$def = internal constant %ie27_str_struct<{[16 x i8] [i8 109, i8 97, i8 105, i8 110, i8 58, i8 68, i8 105, i8 115, i8 107, i8 46, i8 83, i8 116, i8 97, i8 116, i8 115, i8 0]}>
@ie27_str = internal alias i8* bitcast (%ie27_str_struct* @ie27_str$def to i8*)
@ie28_str$def = internal constant %ie28_str_struct<{[16 x i8] [i8 109, i8 97, i8 105, i8 110, i8 58, i8 68, i8 105, i8 115, i8 107, i8 46, i8 83, i8 116, i8 97, i8 116, i8 115, i8 0]}>
@ie28_str = internal alias i8* bitcast (%ie28_str_struct* @ie28_str$def to i8*)
@Disk_Stats_con_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_Stats_con_info$def to i8*)
define ghccc void @Disk_Stats_con_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (%ie27_str_struct* @ie27_str$def to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_Stats_con_info$def to i64)),i64 0), i64 12884901888, i64 1}>
{
ce25:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lne2a = load i64, i64* %R1_Var
  %lne2b = add i64 %lne2a, 1
  store i64 %lne2b, i64* %R1_Var
  %lne2c = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  %lne2d = bitcast i64* %lne2c to i64*
  %lne2e = load i64, i64* %lne2d, !tbaa !1
  %lne2f = inttoptr i64 %lne2e to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lne2g = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lne2f( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lne2g, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@Disk_Stats_static_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_Stats_static_info$def to i8*)
define ghccc void @Disk_Stats_static_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (%ie28_str_struct* @ie28_str$def to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_Stats_static_info$def to i64)),i64 0), i64 12884901888, i64 8}>
{
ce26:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lne2i = load i64, i64* %R1_Var
  %lne2j = add i64 %lne2i, 1
  store i64 %lne2j, i64* %R1_Var
  %lne2k = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  %lne2l = bitcast i64* %lne2k to i64*
  %lne2m = load i64, i64* %lne2l, !tbaa !1
  %lne2n = inttoptr i64 %lne2m to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lne2o = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lne2n( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lne2o, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%ie2r_str_struct = type <{[12 x i8]}>
%ie2s_str_struct = type <{[12 x i8]}>
@ie2r_str$def = internal constant %ie2r_str_struct<{[12 x i8] [i8 109, i8 97, i8 105, i8 110, i8 58, i8 68, i8 105, i8 115, i8 107, i8 46, i8 83, i8 0]}>
@ie2r_str = internal alias i8* bitcast (%ie2r_str_struct* @ie2r_str$def to i8*)
@ie2s_str$def = internal constant %ie2s_str_struct<{[12 x i8] [i8 109, i8 97, i8 105, i8 110, i8 58, i8 68, i8 105, i8 115, i8 107, i8 46, i8 83, i8 0]}>
@ie2s_str = internal alias i8* bitcast (%ie2s_str_struct* @ie2s_str$def to i8*)
@Disk_S_con_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_S_con_info$def to i8*)
define ghccc void @Disk_S_con_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (%ie2r_str_struct* @ie2r_str$def to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_S_con_info$def to i64)),i64 0), i64 4294967299, i64 1}>
{
ce2p:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lne2u = load i64, i64* %R1_Var
  %lne2v = add i64 %lne2u, 1
  store i64 %lne2v, i64* %R1_Var
  %lne2w = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  %lne2x = bitcast i64* %lne2w to i64*
  %lne2y = load i64, i64* %lne2x, !tbaa !1
  %lne2z = inttoptr i64 %lne2y to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lne2A = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lne2z( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lne2A, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@Disk_S_static_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_S_static_info$def to i8*)
define ghccc void @Disk_S_static_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (%ie2s_str_struct* @ie2s_str$def to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Disk_S_static_info$def to i64)),i64 0), i64 4294967299, i64 7}>
{
ce2q:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lne2C = load i64, i64* %R1_Var
  %lne2D = add i64 %lne2C, 1
  store i64 %lne2D, i64* %R1_Var
  %lne2E = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  %lne2F = bitcast i64* %lne2E to i64*
  %lne2G = load i64, i64* %lne2F, !tbaa !1
  %lne2H = inttoptr i64 %lne2G to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lne2I = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lne2H( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lne2I, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Sbl8_srt_struct = type <{i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64}>
@Sbl8_srt$def = internal constant %Sbl8_srt_struct<{i64 ptrtoint (i8* @containerszm0zi5zi7zi1_DataziMapziBase_glue_closure to i64), i64 ptrtoint (i8* @containerszm0zi5zi7zi1_DataziMapziBase_balanceR_closure to i64), i64 ptrtoint (i8* @containerszm0zi5zi7zi1_DataziMapziBase_balanceL_closure to i64), i64 ptrtoint (%Disk_zdsdeletezuzdsgo10_closure_struct* @Disk_zdsdeletezuzdsgo10_closure$def to i64), i64 ptrtoint (%Disk_zdsinsertzuzdsgo4_closure_struct* @Disk_zdsinsertzuzdsgo4_closure$def to i64), i64 ptrtoint (i8* @integerzmgmp_GHCziIntegerziType_plusInteger_closure to i64), i64 ptrtoint (%Disk_zdwvarzualloc_closure_struct* @Disk_zdwvarzualloc_closure$def to i64), i64 ptrtoint (%Disk_varzualloc1_closure_struct* @Disk_varzualloc1_closure$def to i64), i64 ptrtoint (%Disk_varzudelete1_closure_struct* @Disk_varzudelete1_closure$def to i64), i64 ptrtoint (%Disk_varzuset1_closure_struct* @Disk_varzuset1_closure$def to i64), i64 ptrtoint (i8* @base_GHCziErr_error_closure to i64), i64 ptrtoint (%rb0J_closure_struct* @rb0J_closure$def to i64), i64 ptrtoint (%rb0K_closure_struct* @rb0K_closure$def to i64), i64 ptrtoint (%Disk_varzuget2_closure_struct* @Disk_varzuget2_closure$def to i64), i64 ptrtoint (%Disk_varzuget1_closure_struct* @Disk_varzuget1_closure$def to i64), i64 ptrtoint (i8* @base_GHCziIOziHandleziFD_stdout_closure to i64), i64 ptrtoint (%Disk_zdwprintzustats_closure_struct* @Disk_zdwprintzustats_closure$def to i64), i64 ptrtoint (%Disk_printzustats2_closure_struct* @Disk_printzustats2_closure$def to i64), i64 ptrtoint (i8* @base_GHCziIOziHandleziText_hPutStr2_closure to i64), i64 ptrtoint (%Disk_printzustats1_closure_struct* @Disk_printzustats1_closure$def to i64), i64 ptrtoint (%Disk_zdwclosezudisk_closure_struct* @Disk_zdwclosezudisk_closure$def to i64), i64 ptrtoint (i8* @unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_closeFd2_closure to i64), i64 ptrtoint (i8* @base_ForeignziCziError_throwErrno1_closure to i64), i64 ptrtoint (%Disk_closezudisk1_closure_struct* @Disk_closezudisk1_closure$def to i64), i64 ptrtoint (%Disk_zdwsetzunblockszudisk_closure_struct* @Disk_zdwsetzunblockszudisk_closure$def to i64), i64 ptrtoint (i8* @unixzm2zi7zi2zi0_SystemziPosixziFilesziCommon_setFdSizze2_closure to i64), i64 ptrtoint (%Disk_setzunblockszudisk1_closure_struct* @Disk_setzunblockszudisk1_closure$def to i64), i64 ptrtoint (%Disk_initzudiskzucrashlog1_closure_struct* @Disk_initzudiskzucrashlog1_closure$def to i64), i64 ptrtoint (i8* @unixzm2zi7zi2zi0_SystemziPosixziIO_createFile3_closure to i64), i64 ptrtoint (%Disk_initzudisk1_closure_struct* @Disk_initzudisk1_closure$def to i64), i64 ptrtoint (i8* @base_GHCziShow_zdwzdcshowsPrec3_closure to i64), i64 ptrtoint (i8* @integerzmgmp_GHCziIntegerziType_wordToInteger_closure to i64), i64 ptrtoint (%Disk_readzudisk3_closure_struct* @Disk_readzudisk3_closure$def to i64), i64 ptrtoint (%rb0E_closure_struct* @rb0E_closure$def to i64), i64 ptrtoint (i8* @integerzmgmp_GHCziIntegerziType_timesInteger_closure to i64), i64 ptrtoint (%Disk_zdwreadzudisk_closure_struct* @Disk_zdwreadzudisk_closure$def to i64), i64 ptrtoint (i8* @unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_fdSeek2_closure to i64), i64 ptrtoint (%Disk_readzudisk2_closure_struct* @Disk_readzudisk2_closure$def to i64), i64 ptrtoint (i8* @unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_fdRead8_closure to i64), i64 ptrtoint (i8* @base_ForeignziCziError_throwErrnoIfMinus1Retry2_closure to i64), i64 ptrtoint (%Disk_readzudisk1_closure_struct* @Disk_readzudisk1_closure$def to i64), i64 ptrtoint (%rb0P_closure_struct* @rb0P_closure$def to i64), i64 ptrtoint (%rb0Q_closure_struct* @rb0Q_closure$def to i64), i64 ptrtoint (i8* @base_GHCziErr_error_closure to i64), i64 ptrtoint (%rb0A_closure_struct* @rb0A_closure$def to i64), i64 ptrtoint (%rb0S_closure_struct* @rb0S_closure$def to i64), i64 ptrtoint (i8* @Word_zdwi2bs_closure to i64), i64 ptrtoint (%Disk_writezudisk1_closure_struct* @Disk_writezudisk1_closure$def to i64), i64 ptrtoint (i8* @unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_fdWrite2_closure to i64), i64 ptrtoint (%rb0R_closure_struct* @rb0R_closure$def to i64), i64 ptrtoint (i8* @unixzm2zi7zi2zi0_SystemziPosixziUnistd_fileSynchronise2_closure to i64), i64 ptrtoint (%Disk_synczudisk1_closure_struct* @Disk_synczudisk1_closure$def to i64)}>
@Sbl8_srt = internal alias i8* bitcast (%Sbl8_srt_struct* @Sbl8_srt$def to i8*)
@read = external global i8
@integer_gmp_mpn_import = external global i8
@integer_gmp_rscan_nzbyte = external global i8
@close = external global i8
@ghczuwrapperZC23ZCbaseZCSystemziPosixziInternalsZClseek = external global i8
@__hscore_ftruncate = external global i8
@ghczuwrapperZC1ZCunixzm2zi7zi2zi0ZCSystemziPosixziUnistdZCfsync = external global i8
@write = external global i8
@stg_gc_unpt_r1 = external global i8
@integerzmgmp_GHCziIntegerziType_compareInteger_info = external global i8
@containerszm0zi5zi7zi1_DataziMapziBase_Tip_closure = external global i8
@containerszm0zi5zi7zi1_DataziMapziBase_glue_info = external global i8
@containerszm0zi5zi7zi1_DataziMapziBase_balanceL_info = external global i8
@containerszm0zi5zi7zi1_DataziMapziBase_balanceR_info = external global i8
@base_GHCziBase_Nothing_closure = external global i8
@base_GHCziBase_Just_con_info = external global i8
@stg_ap_0_fast = external global i8
@containerszm0zi5zi7zi1_DataziMapziBase_Bin_con_info = external global i8
@ghczmprim_GHCziTypes_False_static_info = external global i8
@ghczmprim_GHCziTypes_True_static_info = external global i8
@dirty_MUT_VAR = external global i8
@ghczmprim_GHCziTuple_Z0T_closure = external global i8
@ghczmprim_GHCziTypes_TrNameS_static_info = external global i8
@ghczmprim_GHCziTypes_Module_static_info = external global i8
@ghczmprim_GHCziTypes_TyCon_static_info = external global i8
@newCAF = external global i8
@stg_bh_upd_frame_info = external global i8
@ghczmprim_GHCziCString_unpackCStringzh_info = external global i8
@ghczmprim_GHCziTypes_Izh_static_info = external global i8
@base_GHCziStackziTypes_SrcLoc_static_info = external global i8
@base_GHCziStackziTypes_PushCallStack_static_info = external global i8
@base_GHCziStackziTypes_EmptyCallStack_closure = external global i8
@integerzmgmp_GHCziIntegerziType_Szh_static_info = external global i8
@stg_upd_frame_info = external global i8
@integerzmgmp_GHCziIntegerziType_plusInteger_info = external global i8
@base_GHCziErr_error_info = external global i8
@ghczmprim_GHCziTypes_ZMZN_closure = external global i8
@base_GHCziShow_zdwshowWord_info = external global i8
@ghczmprim_GHCziTypes_ZC_con_info = external global i8
@stg_gc_pp = external global i8
@ghczmprim_GHCziCString_unpackAppendCStringzh_info = external global i8
@ghczmprim_GHCziTypes_True_closure = external global i8
@base_GHCziIOziHandleziFD_stdout_closure = external global i8
@base_GHCziIOziHandleziText_hPutStr2_info = external global i8
@unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_closeFd2_closure = external global i8
@base_ForeignziCziError_throwErrno1_info = external global i8
@unixzm2zi7zi2zi0_SystemziPosixziFilesziCommon_setFdSizze2_closure = external global i8
@base_GHCziWord_W32zh_static_info = external global i8
@base_GHCziBase_Just_static_info = external global i8
@base_GHCziSTRef_STRef_con_info = external global i8
@unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_defaultFileFlags_closure = external global i8
@unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_ReadWrite_closure = external global i8
@unixzm2zi7zi2zi0_SystemziPosixziIO_createFile3_info = external global i8
@stg_newMutVarzh = external global i8
@ghczmprim_GHCziTuple_Z2T_con_info = external global i8
@base_GHCziShow_zdwzdcshowsPrec3_info = external global i8
@integerzmgmp_GHCziIntegerziType_wordToInteger_info = external global i8
@integerzmgmp_GHCziIntegerziType_smallInteger_info = external global i8
@base_GHCziBase_zpzp_info = external global i8
@Word_W_static_info = external global i8
@Word_bs2i2_closure = external global i8
@suspendThread = external global i8
@resumeThread = external global i8
@base_GHCziInt_I64zh_con_info = external global i8
@stg_gc_unbx_r1 = external global i8
@integerzmgmp_GHCziIntegerziType_zdwbigNatToInteger_info = external global i8
@stg_newPinnedByteArrayzh = external global i8
@integerzmgmp_GHCziIntegerziType_timesInteger_info = external global i8
@integerzmgmp_GHCziIntegerziType_integerToInt_info = external global i8
@unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_fdSeek2_closure = external global i8
@unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_fdRead8_closure = external global i8
@unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_fdRead9_closure = external global i8
@base_ForeignziCziError_throwErrnoIfMinus1Retry2_info = external global i8
@stg_gc_noregs = external global i8
@stg_newByteArrayzh = external global i8
@Word_W_con_info = external global i8
@Word_zdwi2bs_info = external global i8
@unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_fdWrite2_closure = external global i8
@Word_zdWWBS_info = external global i8
@unixzm2zi7zi2zi0_SystemziPosixziUnistd_fileSynchronise2_closure = external global i8
@containerszm0zi5zi7zi1_DataziMapziBase_glue_closure = external global i8
@containerszm0zi5zi7zi1_DataziMapziBase_balanceR_closure = external global i8
@containerszm0zi5zi7zi1_DataziMapziBase_balanceL_closure = external global i8
@integerzmgmp_GHCziIntegerziType_plusInteger_closure = external global i8
@base_GHCziErr_error_closure = external global i8
@base_GHCziIOziHandleziText_hPutStr2_closure = external global i8
@base_ForeignziCziError_throwErrno1_closure = external global i8
@unixzm2zi7zi2zi0_SystemziPosixziIO_createFile3_closure = external global i8
@base_GHCziShow_zdwzdcshowsPrec3_closure = external global i8
@integerzmgmp_GHCziIntegerziType_wordToInteger_closure = external global i8
@integerzmgmp_GHCziIntegerziType_timesInteger_closure = external global i8
@base_ForeignziCziError_throwErrnoIfMinus1Retry2_closure = external global i8
@Word_zdwi2bs_closure = external global i8
@llvm.used = appending constant [157 x i8*] [i8* bitcast (%Sbl8_srt_struct* @Sbl8_srt$def to i8*), i8* bitcast (%ie2s_str_struct* @ie2s_str$def to i8*), i8* bitcast (%ie2r_str_struct* @ie2r_str$def to i8*), i8* bitcast (%ie28_str_struct* @ie28_str$def to i8*), i8* bitcast (%ie27_str_struct* @ie27_str$def to i8*), i8* bitcast (%ie1O_str_struct* @ie1O_str$def to i8*), i8* bitcast (%ie1N_str_struct* @ie1N_str$def to i8*), i8* bitcast (%ie1u_str_struct* @ie1u_str$def to i8*), i8* bitcast (%ie1t_str_struct* @ie1t_str$def to i8*), i8* bitcast (%Disk_S_closure_struct* @Disk_S_closure$def to i8*), i8* bitcast (%Disk_Stats_closure_struct* @Disk_Stats_closure$def to i8*), i8* bitcast (%Disk_FL_closure_struct* @Disk_FL_closure$def to i8*), i8* bitcast (%Disk_VS_closure_struct* @Disk_VS_closure$def to i8*), i8* bitcast (%Disk_trimzudisk_closure_struct* @Disk_trimzudisk_closure$def to i8*), i8* bitcast (%Disk_trimzudisk1_closure_struct* @Disk_trimzudisk1_closure$def to i8*), i8* bitcast (%Disk_synczudisk_closure_struct* @Disk_synczudisk_closure$def to i8*), i8* bitcast (%Disk_synczudisk1_closure_struct* @Disk_synczudisk1_closure$def to i8*), i8* bitcast (%Disk_writezudisk_closure_struct* @Disk_writezudisk_closure$def to i8*), i8* bitcast (%Disk_writezudisk1_closure_struct* @Disk_writezudisk1_closure$def to i8*), i8* bitcast (%cdl6_str_struct* @cdl6_str$def to i8*), i8* bitcast (%cdko_str_struct* @cdko_str$def to i8*), i8* bitcast (%rb0S_closure_struct* @rb0S_closure$def to i8*), i8* bitcast (%rb0R_closure_struct* @rb0R_closure$def to i8*), i8* bitcast (%cdhQ_str_struct* @cdhQ_str$def to i8*), i8* bitcast (%rb0Q_closure_struct* @rb0Q_closure$def to i8*), i8* bitcast (%rb0P_closure_struct* @rb0P_closure$def to i8*), i8* bitcast (%rb0O_closure_struct* @rb0O_closure$def to i8*), i8* bitcast (%rb0N_closure_struct* @rb0N_closure$def to i8*), i8* bitcast (%rb0M_closure_struct* @rb0M_closure$def to i8*), i8* bitcast (%rb0L_closure_struct* @rb0L_closure$def to i8*), i8* bitcast (%Disk_readzudisk_closure_struct* @Disk_readzudisk_closure$def to i8*), i8* bitcast (%Disk_readzudisk1_closure_struct* @Disk_readzudisk1_closure$def to i8*), i8* bitcast (%Disk_zdwreadzudisk_closure_struct* @Disk_zdwreadzudisk_closure$def to i8*), i8* bitcast (%Disk_readzudisk2_closure_struct* @Disk_readzudisk2_closure$def to i8*), i8* bitcast (%ccWl_str_struct* @ccWl_str$def to i8*), i8* bitcast (%ccVD_str_struct* @ccVD_str$def to i8*), i8* bitcast (%Disk_readzudisk3_closure_struct* @Disk_readzudisk3_closure$def to i8*), i8* bitcast (%Disk_readzudisk4_closure_struct* @Disk_readzudisk4_closure$def to i8*), i8* bitcast (%Disk_debugmsg_closure_struct* @Disk_debugmsg_closure$def to i8*), i8* bitcast (%Disk_debugmsg1_closure_struct* @Disk_debugmsg1_closure$def to i8*), i8* bitcast (%Disk_debugmsg2_closure_struct* @Disk_debugmsg2_closure$def to i8*), i8* bitcast (%Disk_bumpRead_closure_struct* @Disk_bumpRead_closure$def to i8*), i8* bitcast (%Disk_bumpRead1_closure_struct* @Disk_bumpRead1_closure$def to i8*), i8* bitcast (%Disk_bumpWrite_closure_struct* @Disk_bumpWrite_closure$def to i8*), i8* bitcast (%Disk_bumpWrite1_closure_struct* @Disk_bumpWrite1_closure$def to i8*), i8* bitcast (%Disk_bumpSync_closure_struct* @Disk_bumpSync_closure$def to i8*), i8* bitcast (%Disk_bumpSync1_closure_struct* @Disk_bumpSync1_closure$def to i8*), i8* bitcast (%Disk_logWrite_closure_struct* @Disk_logWrite_closure$def to i8*), i8* bitcast (%Disk_logWrite1_closure_struct* @Disk_logWrite1_closure$def to i8*), i8* bitcast (%Disk_logFlush_closure_struct* @Disk_logFlush_closure$def to i8*), i8* bitcast (%Disk_logFlush1_closure_struct* @Disk_logFlush1_closure$def to i8*), i8* bitcast (%Disk_getzustats_closure_struct* @Disk_getzustats_closure$def to i8*), i8* bitcast (%Disk_getzustats1_closure_struct* @Disk_getzustats1_closure$def to i8*), i8* bitcast (%Disk_initzudisk_closure_struct* @Disk_initzudisk_closure$def to i8*), i8* bitcast (%Disk_initzudisk1_closure_struct* @Disk_initzudisk1_closure$def to i8*), i8* bitcast (%Disk_initzudiskzucrashlog_closure_struct* @Disk_initzudiskzucrashlog_closure$def to i8*), i8* bitcast (%Disk_initzudiskzucrashlog1_closure_struct* @Disk_initzudiskzucrashlog1_closure$def to i8*), i8* bitcast (%Disk_initzudisk2_closure_struct* @Disk_initzudisk2_closure$def to i8*), i8* bitcast (%Disk_initzudisk3_closure_struct* @Disk_initzudisk3_closure$def to i8*), i8* bitcast (%Disk_initzudisk4_closure_struct* @Disk_initzudisk4_closure$def to i8*), i8* bitcast (%Disk_initzudisk5_closure_struct* @Disk_initzudisk5_closure$def to i8*), i8* bitcast (%Disk_setzunblockszudisk_closure_struct* @Disk_setzunblockszudisk_closure$def to i8*), i8* bitcast (%Disk_setzunblockszudisk1_closure_struct* @Disk_setzunblockszudisk1_closure$def to i8*), i8* bitcast (%Disk_zdwsetzunblockszudisk_closure_struct* @Disk_zdwsetzunblockszudisk_closure$def to i8*), i8* bitcast (%Disk_closezudisk_closure_struct* @Disk_closezudisk_closure$def to i8*), i8* bitcast (%Disk_closezudisk1_closure_struct* @Disk_closezudisk1_closure$def to i8*), i8* bitcast (%Disk_zdwclosezudisk_closure_struct* @Disk_zdwclosezudisk_closure$def to i8*), i8* bitcast (%Disk_getzuflushzulog_closure_struct* @Disk_getzuflushzulog_closure$def to i8*), i8* bitcast (%Disk_getzuflushzulog1_closure_struct* @Disk_getzuflushzulog1_closure$def to i8*), i8* bitcast (%Disk_clearzuflushzulog_closure_struct* @Disk_clearzuflushzulog_closure$def to i8*), i8* bitcast (%Disk_clearzuflushzulog1_closure_struct* @Disk_clearzuflushzulog1_closure$def to i8*), i8* bitcast (%Disk_clearzuflushzulog2_closure_struct* @Disk_clearzuflushzulog2_closure$def to i8*), i8* bitcast (%Disk_printzustats_closure_struct* @Disk_printzustats_closure$def to i8*), i8* bitcast (%Disk_printzustats1_closure_struct* @Disk_printzustats1_closure$def to i8*), i8* bitcast (%cbZW_str_struct* @cbZW_str$def to i8*), i8* bitcast (%cbZx_str_struct* @cbZx_str$def to i8*), i8* bitcast (%cbZ8_str_struct* @cbZ8_str$def to i8*), i8* bitcast (%Disk_zdwprintzustats_closure_struct* @Disk_zdwprintzustats_closure$def to i8*), i8* bitcast (%cbY0_str_struct* @cbY0_str$def to i8*), i8* bitcast (%Disk_printzustats2_closure_struct* @Disk_printzustats2_closure$def to i8*), i8* bitcast (%Disk_varzuget_closure_struct* @Disk_varzuget_closure$def to i8*), i8* bitcast (%Disk_varzuget1_closure_struct* @Disk_varzuget1_closure$def to i8*), i8* bitcast (%Disk_varzuget2_closure_struct* @Disk_varzuget2_closure$def to i8*), i8* bitcast (%cbTA_str_struct* @cbTA_str$def to i8*), i8* bitcast (%rb0K_closure_struct* @rb0K_closure$def to i8*), i8* bitcast (%rb0J_closure_struct* @rb0J_closure$def to i8*), i8* bitcast (%rb0I_closure_struct* @rb0I_closure$def to i8*), i8* bitcast (%rb0H_closure_struct* @rb0H_closure$def to i8*), i8* bitcast (%rb0G_closure_struct* @rb0G_closure$def to i8*), i8* bitcast (%rb0F_closure_struct* @rb0F_closure$def to i8*), i8* bitcast (%Disk_varzuset_closure_struct* @Disk_varzuset_closure$def to i8*), i8* bitcast (%Disk_varzuset1_closure_struct* @Disk_varzuset1_closure$def to i8*), i8* bitcast (%Disk_varzudelete_closure_struct* @Disk_varzudelete_closure$def to i8*), i8* bitcast (%Disk_varzudelete1_closure_struct* @Disk_varzudelete1_closure$def to i8*), i8* bitcast (%Disk_varzualloc_closure_struct* @Disk_varzualloc_closure$def to i8*), i8* bitcast (%Disk_varzualloc1_closure_struct* @Disk_varzualloc1_closure$def to i8*), i8* bitcast (%Disk_zdwvarzualloc_closure_struct* @Disk_zdwvarzualloc_closure$def to i8*), i8* bitcast (%Disk_varzualloc2_closure_struct* @Disk_varzualloc2_closure$def to i8*), i8* bitcast (%rb0E_closure_struct* @rb0E_closure$def to i8*), i8* bitcast (%rb0D_closure_struct* @rb0D_closure$def to i8*), i8* bitcast (%rb0C_closure_struct* @rb0C_closure$def to i8*), i8* bitcast (%rb0B_closure_struct* @rb0B_closure$def to i8*), i8* bitcast (%rb0A_closure_struct* @rb0A_closure$def to i8*), i8* bitcast (%rb0z_closure_struct* @rb0z_closure$def to i8*), i8* bitcast (%cbEq_str_struct* @cbEq_str$def to i8*), i8* bitcast (%rb0y_closure_struct* @rb0y_closure$def to i8*), i8* bitcast (%rb0x_closure_struct* @rb0x_closure$def to i8*), i8* bitcast (%rb0w_closure_struct* @rb0w_closure$def to i8*), i8* bitcast (%rb0v_closure_struct* @rb0v_closure$def to i8*), i8* bitcast (%cbDw_str_struct* @cbDw_str$def to i8*), i8* bitcast (%rb0u_closure_struct* @rb0u_closure$def to i8*), i8* bitcast (%cbCF_str_struct* @cbCF_str$def to i8*), i8* bitcast (%rb0t_closure_struct* @rb0t_closure$def to i8*), i8* bitcast (%cbBO_str_struct* @cbBO_str$def to i8*), i8* bitcast (%rb0s_closure_struct* @rb0s_closure$def to i8*), i8* bitcast (%Disk_zdtcDiskState_closure_struct* @Disk_zdtcDiskState_closure$def to i8*), i8* bitcast (%Disk_zdtcDiskState1_closure_struct* @Disk_zdtcDiskState1_closure$def to i8*), i8* bitcast (%cbBF_str_struct* @cbBF_str$def to i8*), i8* bitcast (%Disk_zdtczqS_closure_struct* @Disk_zdtczqS_closure$def to i8*), i8* bitcast (%Disk_zdtczqS1_closure_struct* @Disk_zdtczqS1_closure$def to i8*), i8* bitcast (%cbBC_str_struct* @cbBC_str$def to i8*), i8* bitcast (%Disk_zdtcVarStore_closure_struct* @Disk_zdtcVarStore_closure$def to i8*), i8* bitcast (%Disk_zdtcVarStore1_closure_struct* @Disk_zdtcVarStore1_closure$def to i8*), i8* bitcast (%cbBz_str_struct* @cbBz_str$def to i8*), i8* bitcast (%Disk_zdtczqVS_closure_struct* @Disk_zdtczqVS_closure$def to i8*), i8* bitcast (%Disk_zdtczqVS1_closure_struct* @Disk_zdtczqVS1_closure$def to i8*), i8* bitcast (%cbBw_str_struct* @cbBw_str$def to i8*), i8* bitcast (%Disk_zdtcFlushLog_closure_struct* @Disk_zdtcFlushLog_closure$def to i8*), i8* bitcast (%Disk_zdtcFlushLog1_closure_struct* @Disk_zdtcFlushLog1_closure$def to i8*), i8* bitcast (%cbBt_str_struct* @cbBt_str$def to i8*), i8* bitcast (%Disk_zdtczqFL_closure_struct* @Disk_zdtczqFL_closure$def to i8*), i8* bitcast (%Disk_zdtczqFL1_closure_struct* @Disk_zdtczqFL1_closure$def to i8*), i8* bitcast (%cbBq_str_struct* @cbBq_str$def to i8*), i8* bitcast (%Disk_zdtcDiskStats_closure_struct* @Disk_zdtcDiskStats_closure$def to i8*), i8* bitcast (%Disk_zdtcDiskStats1_closure_struct* @Disk_zdtcDiskStats1_closure$def to i8*), i8* bitcast (%cbBn_str_struct* @cbBn_str$def to i8*), i8* bitcast (%Disk_zdtczqStats_closure_struct* @Disk_zdtczqStats_closure$def to i8*), i8* bitcast (%Disk_zdtczqStats1_closure_struct* @Disk_zdtczqStats1_closure$def to i8*), i8* bitcast (%cbBk_str_struct* @cbBk_str$def to i8*), i8* bitcast (%Disk_zdtrModule_closure_struct* @Disk_zdtrModule_closure$def to i8*), i8* bitcast (%Disk_zdtrModule1_closure_struct* @Disk_zdtrModule1_closure$def to i8*), i8* bitcast (%cbBh_str_struct* @cbBh_str$def to i8*), i8* bitcast (%Disk_zdtrModule2_closure_struct* @Disk_zdtrModule2_closure$def to i8*), i8* bitcast (%cbBf_str_struct* @cbBf_str$def to i8*), i8* bitcast (%Disk_clearzustats_closure_struct* @Disk_clearzustats_closure$def to i8*), i8* bitcast (%Disk_clearzustats1_closure_struct* @Disk_clearzustats1_closure$def to i8*), i8* bitcast (%Disk_clearzustats2_closure_struct* @Disk_clearzustats2_closure$def to i8*), i8* bitcast (%Disk_debugSyncs_closure_struct* @Disk_debugSyncs_closure$def to i8*), i8* bitcast (%Disk_reallySync_closure_struct* @Disk_reallySync_closure$def to i8*), i8* bitcast (%Disk_verbose_closure_struct* @Disk_verbose_closure$def to i8*), i8* bitcast (%Disk_zdsinsertzuzdsgo4_closure_struct* @Disk_zdsinsertzuzdsgo4_closure$def to i8*), i8* bitcast (%Disk_zdslookup1_closure_struct* @Disk_zdslookup1_closure$def to i8*), i8* bitcast (%Disk_zdsdeletezuzdsgo10_closure_struct* @Disk_zdsdeletezuzdsgo10_closure$def to i8*), i8* bitcast (%Disk_zdWS_closure_struct* @Disk_zdWS_closure$def to i8*), i8* bitcast (%Disk_zdWStats_closure_struct* @Disk_zdWStats_closure$def to i8*), i8* bitcast (%Disk_zdWFL_closure_struct* @Disk_zdWFL_closure$def to i8*), i8* bitcast (%__stginit_Disk_struct* @__stginit_Disk$def to i8*)], section "llvm.metadata"
