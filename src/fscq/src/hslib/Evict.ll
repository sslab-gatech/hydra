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
%__stginit_Evict_struct = type <{}>
@__stginit_Evict$def = internal global %__stginit_Evict_struct<{}>
@__stginit_Evict = alias i8* bitcast (%__stginit_Evict_struct* @__stginit_Evict$def to i8*)
%Evict_evictionzuinit_closure_struct = type <{i64}>
@Evict_evictionzuinit_closure$def = internal global %Evict_evictionzuinit_closure_struct<{i64 ptrtoint (i8* @Evict_LRU_static_info to i64)}>
@Evict_evictionzuinit_closure = alias i8* bitcast (%Evict_evictionzuinit_closure_struct* @Evict_evictionzuinit_closure$def to i8*)
%Evict_evictionzuupdate_closure_struct = type <{i64}>
@Evict_evictionzuupdate_closure$def = internal global %Evict_evictionzuupdate_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Evict_evictionzuupdate_info$def to i64)}>
@Evict_evictionzuupdate_closure = alias i8* bitcast (%Evict_evictionzuupdate_closure_struct* @Evict_evictionzuupdate_closure$def to i8*)
@Evict_evictionzuupdate_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Evict_evictionzuupdate_info$def to i8*)
define ghccc void @Evict_evictionzuupdate_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 8589934607, i64 0, i64 15}>
{
c1w8:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %ln1wb = and i64 %R2_Arg, -8
  store i64 %ln1wb, i64* %R1_Var
  %ln1wd = load i64, i64* %R1_Var
  %ln1we = inttoptr i64 %ln1wd to i64*
  %ln1wf = load i64, i64* %ln1we, !tbaa !3
  %ln1wg = inttoptr i64 %ln1wf to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1wh = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1wg( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %ln1wh, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Evict_evictionzuchoose1_closure_struct = type <{i64, i64}>
@Evict_evictionzuchoose1_closure$def = internal global %Evict_evictionzuchoose1_closure_struct<{i64 ptrtoint (i8* @integerzmgmp_GHCziIntegerziType_Szh_static_info to i64), i64 0}>
@Evict_evictionzuchoose1_closure = alias i8* bitcast (%Evict_evictionzuchoose1_closure_struct* @Evict_evictionzuchoose1_closure$def to i8*)
%Evict_evictionzuchoose_closure_struct = type <{i64}>
@Evict_evictionzuchoose_closure$def = internal global %Evict_evictionzuchoose_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Evict_evictionzuchoose_info$def to i64)}>
@Evict_evictionzuchoose_closure = alias i8* bitcast (%Evict_evictionzuchoose_closure_struct* @Evict_evictionzuchoose_closure$def to i8*)
@Evict_evictionzuchoose_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Evict_evictionzuchoose_info$def to i8*)
define ghccc void @Evict_evictionzuchoose_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 4294967301, i64 0, i64 15}>
{
c1wp:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %ln1wu = load i64*, i64** %Hp_Var
  %ln1wv = getelementptr inbounds i64, i64* %ln1wu, i32 3
  %ln1ww = ptrtoint i64* %ln1wv to i64
  %ln1wx = inttoptr i64 %ln1ww to i64*
  store i64* %ln1wx, i64** %Hp_Var
  %ln1wy = load i64*, i64** %Hp_Var
  %ln1wz = ptrtoint i64* %ln1wy to i64
  %ln1wA = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %ln1wB = bitcast i64* %ln1wA to i64*
  %ln1wC = load i64, i64* %ln1wB, !tbaa !4
  %ln1wD = icmp ugt i64 %ln1wz, %ln1wC
  br i1 %ln1wD, label %c1wt, label %c1ws
c1ws:
  %ln1wF = ptrtoint i8* @ghczmprim_GHCziTuple_Z2T_con_info to i64
  %ln1wE = load i64*, i64** %Hp_Var
  %ln1wG = getelementptr inbounds i64, i64* %ln1wE, i32 -2
  store i64 %ln1wF, i64* %ln1wG, !tbaa !2
  %ln1wI = ptrtoint %Evict_evictionzuchoose1_closure_struct* @Evict_evictionzuchoose1_closure$def to i64
  %ln1wJ = add i64 %ln1wI, 1
  %ln1wH = load i64*, i64** %Hp_Var
  %ln1wK = getelementptr inbounds i64, i64* %ln1wH, i32 -1
  store i64 %ln1wJ, i64* %ln1wK, !tbaa !2
  %ln1wM = load i64, i64* %R2_Var
  %ln1wL = load i64*, i64** %Hp_Var
  %ln1wN = getelementptr inbounds i64, i64* %ln1wL, i32 0
  store i64 %ln1wM, i64* %ln1wN, !tbaa !2
  %ln1wP = load i64*, i64** %Hp_Var
  %ln1wQ = ptrtoint i64* %ln1wP to i64
  %ln1wR = add i64 %ln1wQ, -15
  store i64 %ln1wR, i64* %R1_Var
  %ln1wS = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  %ln1wT = bitcast i64* %ln1wS to i64*
  %ln1wU = load i64, i64* %ln1wT, !tbaa !1
  %ln1wV = inttoptr i64 %ln1wU to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1wW = load i64*, i64** %Hp_Var
  %ln1wX = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1wV( i64* %Base_Arg, i64* %Sp_Arg, i64* %ln1wW, i64 %ln1wX, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
c1wt:
  %ln1wY = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %ln1wY, !tbaa !4
  %ln1wZ = load i64, i64* %R2_Var
  store i64 %ln1wZ, i64* %R2_Var
  %ln1x0 = ptrtoint %Evict_evictionzuchoose_closure_struct* @Evict_evictionzuchoose_closure$def to i64
  store i64 %ln1x0, i64* %R1_Var
  %ln1x1 = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %ln1x2 = bitcast i64* %ln1x1 to i64*
  %ln1x3 = load i64, i64* %ln1x2, !tbaa !4
  %ln1x4 = inttoptr i64 %ln1x3 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1x5 = load i64*, i64** %Hp_Var
  %ln1x6 = load i64, i64* %R1_Var
  %ln1x7 = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1x4( i64* %Base_Arg, i64* %Sp_Arg, i64* %ln1x5, i64 %ln1x6, i64 %ln1x7, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%c1x9_str_struct = type <{[5 x i8]}>
%Evict_zdtrModule2_closure_struct = type <{i64, i64}>
@c1x9_str$def = internal constant %c1x9_str_struct<{[5 x i8] [i8 109, i8 97, i8 105, i8 110, i8 0]}>
@c1x9_str = internal alias i8* bitcast (%c1x9_str_struct* @c1x9_str$def to i8*)
@Evict_zdtrModule2_closure$def = internal global %Evict_zdtrModule2_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TrNameS_static_info to i64), i64 ptrtoint (i8* @c1x9_str to i64)}>
@Evict_zdtrModule2_closure = alias i8* bitcast (%Evict_zdtrModule2_closure_struct* @Evict_zdtrModule2_closure$def to i8*)
%c1xb_str_struct = type <{[6 x i8]}>
%Evict_zdtrModule1_closure_struct = type <{i64, i64}>
@c1xb_str$def = internal constant %c1xb_str_struct<{[6 x i8] [i8 69, i8 118, i8 105, i8 99, i8 116, i8 0]}>
@c1xb_str = internal alias i8* bitcast (%c1xb_str_struct* @c1xb_str$def to i8*)
@Evict_zdtrModule1_closure$def = internal global %Evict_zdtrModule1_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TrNameS_static_info to i64), i64 ptrtoint (i8* @c1xb_str to i64)}>
@Evict_zdtrModule1_closure = alias i8* bitcast (%Evict_zdtrModule1_closure_struct* @Evict_zdtrModule1_closure$def to i8*)
%Evict_zdtrModule_closure_struct = type <{i64, i64, i64, i64}>
@Evict_zdtrModule_closure$def = internal global %Evict_zdtrModule_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_Module_static_info to i64), i64 add (i64 ptrtoint (%Evict_zdtrModule2_closure_struct* @Evict_zdtrModule2_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%Evict_zdtrModule1_closure_struct* @Evict_zdtrModule1_closure$def to i64),i64 1), i64 3}>
@Evict_zdtrModule_closure = alias i8* bitcast (%Evict_zdtrModule_closure_struct* @Evict_zdtrModule_closure$def to i8*)
%c1xe_str_struct = type <{[5 x i8]}>
%Evict_zdtczqLRU1_closure_struct = type <{i64, i64}>
@c1xe_str$def = internal constant %c1xe_str_struct<{[5 x i8] [i8 39, i8 76, i8 82, i8 85, i8 0]}>
@c1xe_str = internal alias i8* bitcast (%c1xe_str_struct* @c1xe_str$def to i8*)
@Evict_zdtczqLRU1_closure$def = internal global %Evict_zdtczqLRU1_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TrNameS_static_info to i64), i64 ptrtoint (i8* @c1xe_str to i64)}>
@Evict_zdtczqLRU1_closure = alias i8* bitcast (%Evict_zdtczqLRU1_closure_struct* @Evict_zdtczqLRU1_closure$def to i8*)
%Evict_zdtczqLRU_closure_struct = type <{i64, i64, i64, i64, i64, i64}>
@Evict_zdtczqLRU_closure$def = internal global %Evict_zdtczqLRU_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TyCon_static_info to i64), i64 add (i64 ptrtoint (%Evict_zdtrModule_closure_struct* @Evict_zdtrModule_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%Evict_zdtczqLRU1_closure_struct* @Evict_zdtczqLRU1_closure$def to i64),i64 1), i64 2972429665803681475, i64 -9040111219773721704, i64 3}>
@Evict_zdtczqLRU_closure = alias i8* bitcast (%Evict_zdtczqLRU_closure_struct* @Evict_zdtczqLRU_closure$def to i8*)
%c1xh_str_struct = type <{[14 x i8]}>
%Evict_zdtcEvictionState1_closure_struct = type <{i64, i64}>
@c1xh_str$def = internal constant %c1xh_str_struct<{[14 x i8] [i8 69, i8 118, i8 105, i8 99, i8 116, i8 105, i8 111, i8 110, i8 83, i8 116, i8 97, i8 116, i8 101, i8 0]}>
@c1xh_str = internal alias i8* bitcast (%c1xh_str_struct* @c1xh_str$def to i8*)
@Evict_zdtcEvictionState1_closure$def = internal global %Evict_zdtcEvictionState1_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TrNameS_static_info to i64), i64 ptrtoint (i8* @c1xh_str to i64)}>
@Evict_zdtcEvictionState1_closure = alias i8* bitcast (%Evict_zdtcEvictionState1_closure_struct* @Evict_zdtcEvictionState1_closure$def to i8*)
%Evict_zdtcEvictionState_closure_struct = type <{i64, i64, i64, i64, i64, i64}>
@Evict_zdtcEvictionState_closure$def = internal global %Evict_zdtcEvictionState_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TyCon_static_info to i64), i64 add (i64 ptrtoint (%Evict_zdtrModule_closure_struct* @Evict_zdtrModule_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%Evict_zdtcEvictionState1_closure_struct* @Evict_zdtcEvictionState1_closure$def to i64),i64 1), i64 -6836147115427055168, i64 -118735345585002454, i64 3}>
@Evict_zdtcEvictionState_closure = alias i8* bitcast (%Evict_zdtcEvictionState_closure_struct* @Evict_zdtcEvictionState_closure$def to i8*)
%Evict_LRU_closure_struct = type <{i64}>
@Evict_LRU_closure$def = internal global %Evict_LRU_closure_struct<{i64 ptrtoint (i8* @Evict_LRU_static_info to i64)}>
@Evict_LRU_closure = alias i8* bitcast (%Evict_LRU_closure_struct* @Evict_LRU_closure$def to i8*)
%Evict_EvictionState_closure_tbl_struct = type <{i64}>
@Evict_EvictionState_closure_tbl$def = internal constant %Evict_EvictionState_closure_tbl_struct<{i64 add (i64 ptrtoint (%Evict_LRU_closure_struct* @Evict_LRU_closure$def to i64),i64 1)}>
@Evict_EvictionState_closure_tbl = alias i8* bitcast (%Evict_EvictionState_closure_tbl_struct* @Evict_EvictionState_closure_tbl$def to i8*)
%i1xm_str_struct = type <{[15 x i8]}>
@i1xm_str$def = internal constant %i1xm_str_struct<{[15 x i8] [i8 109, i8 97, i8 105, i8 110, i8 58, i8 69, i8 118, i8 105, i8 99, i8 116, i8 46, i8 76, i8 82, i8 85, i8 0]}>
@i1xm_str = internal alias i8* bitcast (%i1xm_str_struct* @i1xm_str$def to i8*)
@Evict_LRU_static_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Evict_LRU_static_info$def to i8*)
define ghccc void @Evict_LRU_static_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (%i1xm_str_struct* @i1xm_str$def to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Evict_LRU_static_info$def to i64)),i64 0), i64 0, i64 8}>
{
c1xl:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %ln1xo = load i64, i64* %R1_Var
  %ln1xp = add i64 %ln1xo, 1
  store i64 %ln1xp, i64* %R1_Var
  %ln1xq = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  %ln1xr = bitcast i64* %ln1xq to i64*
  %ln1xs = load i64, i64* %ln1xr, !tbaa !1
  %ln1xt = inttoptr i64 %ln1xs to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1xu = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1xt( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %ln1xu, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%S1xv_srt_struct = type <{}>
@S1xv_srt$def = internal constant %S1xv_srt_struct<{}>
@S1xv_srt = internal alias i8* bitcast (%S1xv_srt_struct* @S1xv_srt$def to i8*)
@ghczmprim_GHCziTypes_TrNameS_static_info = external global i8
@ghczmprim_GHCziTypes_Module_static_info = external global i8
@integerzmgmp_GHCziIntegerziType_Szh_static_info = external global i8
@ghczmprim_GHCziTuple_Z2T_con_info = external global i8
@ghczmprim_GHCziTypes_TyCon_static_info = external global i8
@llvm.used = appending constant [20 x i8*] [i8* bitcast (%S1xv_srt_struct* @S1xv_srt$def to i8*), i8* bitcast (%i1xm_str_struct* @i1xm_str$def to i8*), i8* bitcast (%Evict_EvictionState_closure_tbl_struct* @Evict_EvictionState_closure_tbl$def to i8*), i8* bitcast (%Evict_LRU_closure_struct* @Evict_LRU_closure$def to i8*), i8* bitcast (%Evict_zdtcEvictionState_closure_struct* @Evict_zdtcEvictionState_closure$def to i8*), i8* bitcast (%Evict_zdtcEvictionState1_closure_struct* @Evict_zdtcEvictionState1_closure$def to i8*), i8* bitcast (%c1xh_str_struct* @c1xh_str$def to i8*), i8* bitcast (%Evict_zdtczqLRU_closure_struct* @Evict_zdtczqLRU_closure$def to i8*), i8* bitcast (%Evict_zdtczqLRU1_closure_struct* @Evict_zdtczqLRU1_closure$def to i8*), i8* bitcast (%c1xe_str_struct* @c1xe_str$def to i8*), i8* bitcast (%Evict_zdtrModule_closure_struct* @Evict_zdtrModule_closure$def to i8*), i8* bitcast (%Evict_zdtrModule1_closure_struct* @Evict_zdtrModule1_closure$def to i8*), i8* bitcast (%c1xb_str_struct* @c1xb_str$def to i8*), i8* bitcast (%Evict_zdtrModule2_closure_struct* @Evict_zdtrModule2_closure$def to i8*), i8* bitcast (%c1x9_str_struct* @c1x9_str$def to i8*), i8* bitcast (%Evict_evictionzuchoose_closure_struct* @Evict_evictionzuchoose_closure$def to i8*), i8* bitcast (%Evict_evictionzuchoose1_closure_struct* @Evict_evictionzuchoose1_closure$def to i8*), i8* bitcast (%Evict_evictionzuupdate_closure_struct* @Evict_evictionzuupdate_closure$def to i8*), i8* bitcast (%Evict_evictionzuinit_closure_struct* @Evict_evictionzuinit_closure$def to i8*), i8* bitcast (%__stginit_Evict_struct* @__stginit_Evict$def to i8*)], section "llvm.metadata"
