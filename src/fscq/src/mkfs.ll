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
%__stginit_Main_struct = type <{}>
@__stginit_Main$def = internal global %__stginit_Main_struct<{}>
@__stginit_Main = alias i8* bitcast (%__stginit_Main_struct* @__stginit_Main$def to i8*)
%Main_cachesizze_closure_struct = type <{i64, i64}>
@Main_cachesizze_closure$def = internal global %Main_cachesizze_closure_struct<{i64 ptrtoint (i8* @integerzmgmp_GHCziIntegerziType_Szh_static_info to i64), i64 100000}>
@Main_cachesizze_closure = alias i8* bitcast (%Main_cachesizze_closure_struct* @Main_cachesizze_closure$def to i8*)
%ccOQ_str_struct = type <{[5 x i8]}>
%Main_zdtrModule2_closure_struct = type <{i64, i64}>
@ccOQ_str$def = internal constant %ccOQ_str_struct<{[5 x i8] [i8 109, i8 97, i8 105, i8 110, i8 0]}>
@ccOQ_str = internal alias i8* bitcast (%ccOQ_str_struct* @ccOQ_str$def to i8*)
@Main_zdtrModule2_closure$def = internal global %Main_zdtrModule2_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TrNameS_static_info to i64), i64 ptrtoint (i8* @ccOQ_str to i64)}>
@Main_zdtrModule2_closure = alias i8* bitcast (%Main_zdtrModule2_closure_struct* @Main_zdtrModule2_closure$def to i8*)
%ccOS_str_struct = type <{[5 x i8]}>
%Main_zdtrModule1_closure_struct = type <{i64, i64}>
@ccOS_str$def = internal constant %ccOS_str_struct<{[5 x i8] [i8 77, i8 97, i8 105, i8 110, i8 0]}>
@ccOS_str = internal alias i8* bitcast (%ccOS_str_struct* @ccOS_str$def to i8*)
@Main_zdtrModule1_closure$def = internal global %Main_zdtrModule1_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TrNameS_static_info to i64), i64 ptrtoint (i8* @ccOS_str to i64)}>
@Main_zdtrModule1_closure = alias i8* bitcast (%Main_zdtrModule1_closure_struct* @Main_zdtrModule1_closure$def to i8*)
%Main_zdtrModule_closure_struct = type <{i64, i64, i64, i64}>
@Main_zdtrModule_closure$def = internal global %Main_zdtrModule_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_Module_static_info to i64), i64 add (i64 ptrtoint (%Main_zdtrModule2_closure_struct* @Main_zdtrModule2_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%Main_zdtrModule1_closure_struct* @Main_zdtrModule1_closure$def to i64),i64 1), i64 3}>
@Main_zdtrModule_closure = alias i8* bitcast (%Main_zdtrModule_closure_struct* @Main_zdtrModule_closure$def to i8*)
%rcN8_closure_struct = type <{i64, i64, i64, i64}>
%ccP1_str_struct = type <{[21 x i8]}>
@rcN8_closure$def = internal global %rcN8_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rcN8_info$def to i64), i64 0, i64 0, i64 0}>
@rcN8_closure = internal alias i8* bitcast (%rcN8_closure_struct* @rcN8_closure$def to i8*)
@ccP1_str$def = internal constant %ccP1_str_struct<{[21 x i8] [i8 85, i8 115, i8 97, i8 103, i8 101, i8 58, i8 32, i8 109, i8 107, i8 102, i8 115, i8 32, i8 100, i8 105, i8 115, i8 107, i8 112, i8 97, i8 116, i8 104, i8 0]}>
@ccP1_str = internal alias i8* bitcast (%ccP1_str_struct* @ccP1_str$def to i8*)
@rcN8_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rcN8_info$def to i8*)
define internal ghccc void @rcN8_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 22}>
{
ccP2:
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
  %lccOY = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lncP5 = load i64*, i64** %Sp_Var
  %lncP6 = getelementptr inbounds i64, i64* %lncP5, i32 -2
  %lncP7 = ptrtoint i64* %lncP6 to i64
  %lncP8 = icmp ult i64 %lncP7, %SpLim_Arg
  br i1 %lncP8, label %ccP3, label %ccP4
ccP4:
  %lncP9 = ptrtoint i64* %Base_Arg to i64
  %lncPa = inttoptr i64 %lncP9 to i8*
  %lncPb = load i64, i64* %R1_Var
  %lncPc = inttoptr i64 %lncPb to i8*
  %lncPd = bitcast i8* @newCAF to i8* (i8*, i8*)*
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
  %lncPe = call ccc i8* (i8*, i8*) %lncPd( i8* %lncPa, i8* %lncPc ) nounwind
  %lncPf = ptrtoint i8* %lncPe to i64
  store i64 %lncPf, i64* %lccOY
  %lncPg = load i64, i64* %lccOY
  %lncPh = icmp eq i64 %lncPg, 0
  br i1 %lncPh, label %ccP0, label %ccOZ
ccOZ:
  %lncPj = ptrtoint i8* @stg_bh_upd_frame_info to i64
  %lncPi = load i64*, i64** %Sp_Var
  %lncPk = getelementptr inbounds i64, i64* %lncPi, i32 -2
  store i64 %lncPj, i64* %lncPk, !tbaa !1
  %lncPm = load i64, i64* %lccOY
  %lncPl = load i64*, i64** %Sp_Var
  %lncPn = getelementptr inbounds i64, i64* %lncPl, i32 -1
  store i64 %lncPm, i64* %lncPn, !tbaa !1
  %lncPo = ptrtoint %ccP1_str_struct* @ccP1_str$def to i64
  store i64 %lncPo, i64* %R2_Var
  %lncPp = load i64*, i64** %Sp_Var
  %lncPq = getelementptr inbounds i64, i64* %lncPp, i32 -2
  %lncPr = ptrtoint i64* %lncPq to i64
  %lncPs = inttoptr i64 %lncPr to i64*
  store i64* %lncPs, i64** %Sp_Var
  %lncPt = bitcast i8* @ghczmprim_GHCziCString_unpackCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncPu = load i64*, i64** %Sp_Var
  %lncPv = load i64, i64* %R1_Var
  %lncPw = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncPt( i64* %Base_Arg, i64* %lncPu, i64* %Hp_Arg, i64 %lncPv, i64 %lncPw, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccP0:
  %lncPy = load i64, i64* %R1_Var
  %lncPz = inttoptr i64 %lncPy to i64*
  %lncPA = load i64, i64* %lncPz, !tbaa !3
  %lncPB = inttoptr i64 %lncPA to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncPC = load i64*, i64** %Sp_Var
  %lncPD = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncPB( i64* %Base_Arg, i64* %lncPC, i64* %Hp_Arg, i64 %lncPD, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccP3:
  %lncPE = load i64, i64* %R1_Var
  store i64 %lncPE, i64* %R1_Var
  %lncPF = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lncPG = bitcast i64* %lncPF to i64*
  %lncPH = load i64, i64* %lncPG, !tbaa !4
  %lncPI = inttoptr i64 %lncPH to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncPJ = load i64*, i64** %Sp_Var
  %lncPK = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncPI( i64* %Base_Arg, i64* %lncPJ, i64* %Hp_Arg, i64 %lncPK, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%rcN9_closure_struct = type <{i64, i64}>
@rcN9_closure$def = internal global %rcN9_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rcN9_info$def to i64), i64 0}>
@rcN9_closure = internal alias i8* bitcast (%rcN9_closure_struct* @rcN9_closure$def to i8*)
@rcN9_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rcN9_info$def to i8*)
define internal ghccc void @rcN9_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rcN9_info$def to i64)),i64 0), i64 4294967299, i64 0, i64 30064771087}>
{
ccPQ:
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %lncPU = ptrtoint i8* @ghczmprim_GHCziTypes_True_closure to i64
  %lncPV = add i64 %lncPU, 2
  store i64 %lncPV, i64* %R4_Var
  %lncPW = ptrtoint %rcN8_closure_struct* @rcN8_closure$def to i64
  store i64 %lncPW, i64* %R3_Var
  %lncPX = ptrtoint i8* @base_GHCziIOziHandleziFD_stdout_closure to i64
  store i64 %lncPX, i64* %R2_Var
  %lncPY = bitcast i8* @base_GHCziIOziHandleziText_hPutStr2_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncPZ = load i64, i64* %R2_Var
  %lncQ0 = load i64, i64* %R3_Var
  %lncQ1 = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncPY( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lncPZ, i64 %lncQ0, i64 %lncQ1, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%rcNa_closure_struct = type <{i64, i64, i64, i64}>
%ccQ9_str_struct = type <{[8 x i8]}>
@rcNa_closure$def = internal global %rcNa_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rcNa_info$def to i64), i64 0, i64 0, i64 0}>
@rcNa_closure = internal alias i8* bitcast (%rcNa_closure_struct* @rcNa_closure$def to i8*)
@ccQ9_str$def = internal constant %ccQ9_str_struct<{[8 x i8] [i8 32, i8 98, i8 108, i8 111, i8 99, i8 107, i8 115, i8 0]}>
@ccQ9_str = internal alias i8* bitcast (%ccQ9_str_struct* @ccQ9_str$def to i8*)
@rcNa_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rcNa_info$def to i8*)
define internal ghccc void @rcNa_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 22}>
{
ccQa:
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
  %lccQ6 = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lncQd = load i64*, i64** %Sp_Var
  %lncQe = getelementptr inbounds i64, i64* %lncQd, i32 -2
  %lncQf = ptrtoint i64* %lncQe to i64
  %lncQg = icmp ult i64 %lncQf, %SpLim_Arg
  br i1 %lncQg, label %ccQb, label %ccQc
ccQc:
  %lncQh = ptrtoint i64* %Base_Arg to i64
  %lncQi = inttoptr i64 %lncQh to i8*
  %lncQj = load i64, i64* %R1_Var
  %lncQk = inttoptr i64 %lncQj to i8*
  %lncQl = bitcast i8* @newCAF to i8* (i8*, i8*)*
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
  %lncQm = call ccc i8* (i8*, i8*) %lncQl( i8* %lncQi, i8* %lncQk ) nounwind
  %lncQn = ptrtoint i8* %lncQm to i64
  store i64 %lncQn, i64* %lccQ6
  %lncQo = load i64, i64* %lccQ6
  %lncQp = icmp eq i64 %lncQo, 0
  br i1 %lncQp, label %ccQ8, label %ccQ7
ccQ7:
  %lncQr = ptrtoint i8* @stg_bh_upd_frame_info to i64
  %lncQq = load i64*, i64** %Sp_Var
  %lncQs = getelementptr inbounds i64, i64* %lncQq, i32 -2
  store i64 %lncQr, i64* %lncQs, !tbaa !1
  %lncQu = load i64, i64* %lccQ6
  %lncQt = load i64*, i64** %Sp_Var
  %lncQv = getelementptr inbounds i64, i64* %lncQt, i32 -1
  store i64 %lncQu, i64* %lncQv, !tbaa !1
  %lncQw = ptrtoint %ccQ9_str_struct* @ccQ9_str$def to i64
  store i64 %lncQw, i64* %R2_Var
  %lncQx = load i64*, i64** %Sp_Var
  %lncQy = getelementptr inbounds i64, i64* %lncQx, i32 -2
  %lncQz = ptrtoint i64* %lncQy to i64
  %lncQA = inttoptr i64 %lncQz to i64*
  store i64* %lncQA, i64** %Sp_Var
  %lncQB = bitcast i8* @ghczmprim_GHCziCString_unpackCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncQC = load i64*, i64** %Sp_Var
  %lncQD = load i64, i64* %R1_Var
  %lncQE = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncQB( i64* %Base_Arg, i64* %lncQC, i64* %Hp_Arg, i64 %lncQD, i64 %lncQE, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccQ8:
  %lncQG = load i64, i64* %R1_Var
  %lncQH = inttoptr i64 %lncQG to i64*
  %lncQI = load i64, i64* %lncQH, !tbaa !3
  %lncQJ = inttoptr i64 %lncQI to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncQK = load i64*, i64** %Sp_Var
  %lncQL = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncQJ( i64* %Base_Arg, i64* %lncQK, i64* %Hp_Arg, i64 %lncQL, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccQb:
  %lncQM = load i64, i64* %R1_Var
  store i64 %lncQM, i64* %R1_Var
  %lncQN = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lncQO = bitcast i64* %lncQN to i64*
  %lncQP = load i64, i64* %lncQO, !tbaa !4
  %lncQQ = inttoptr i64 %lncQP to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncQR = load i64*, i64** %Sp_Var
  %lncQS = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncQQ( i64* %Base_Arg, i64* %lncQR, i64* %Hp_Arg, i64 %lncQS, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%rcNb_closure_struct = type <{i64, i64, i64, i64}>
%ccR0_str_struct = type <{[6 x i8]}>
@rcNb_closure$def = internal global %rcNb_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rcNb_info$def to i64), i64 0, i64 0, i64 0}>
@rcNb_closure = internal alias i8* bitcast (%rcNb_closure_struct* @rcNb_closure$def to i8*)
@ccR0_str$def = internal constant %ccR0_str_struct<{[6 x i8] [i8 101, i8 114, i8 114, i8 111, i8 114, i8 0]}>
@ccR0_str = internal alias i8* bitcast (%ccR0_str_struct* @ccR0_str$def to i8*)
@rcNb_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rcNb_info$def to i8*)
define internal ghccc void @rcNb_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 22}>
{
ccR1:
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
  %lccQX = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lncR4 = load i64*, i64** %Sp_Var
  %lncR5 = getelementptr inbounds i64, i64* %lncR4, i32 -2
  %lncR6 = ptrtoint i64* %lncR5 to i64
  %lncR7 = icmp ult i64 %lncR6, %SpLim_Arg
  br i1 %lncR7, label %ccR2, label %ccR3
ccR3:
  %lncR8 = ptrtoint i64* %Base_Arg to i64
  %lncR9 = inttoptr i64 %lncR8 to i8*
  %lncRa = load i64, i64* %R1_Var
  %lncRb = inttoptr i64 %lncRa to i8*
  %lncRc = bitcast i8* @newCAF to i8* (i8*, i8*)*
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
  %lncRd = call ccc i8* (i8*, i8*) %lncRc( i8* %lncR9, i8* %lncRb ) nounwind
  %lncRe = ptrtoint i8* %lncRd to i64
  store i64 %lncRe, i64* %lccQX
  %lncRf = load i64, i64* %lccQX
  %lncRg = icmp eq i64 %lncRf, 0
  br i1 %lncRg, label %ccQZ, label %ccQY
ccQY:
  %lncRi = ptrtoint i8* @stg_bh_upd_frame_info to i64
  %lncRh = load i64*, i64** %Sp_Var
  %lncRj = getelementptr inbounds i64, i64* %lncRh, i32 -2
  store i64 %lncRi, i64* %lncRj, !tbaa !1
  %lncRl = load i64, i64* %lccQX
  %lncRk = load i64*, i64** %Sp_Var
  %lncRm = getelementptr inbounds i64, i64* %lncRk, i32 -1
  store i64 %lncRl, i64* %lncRm, !tbaa !1
  %lncRn = ptrtoint %ccR0_str_struct* @ccR0_str$def to i64
  store i64 %lncRn, i64* %R2_Var
  %lncRo = load i64*, i64** %Sp_Var
  %lncRp = getelementptr inbounds i64, i64* %lncRo, i32 -2
  %lncRq = ptrtoint i64* %lncRp to i64
  %lncRr = inttoptr i64 %lncRq to i64*
  store i64* %lncRr, i64** %Sp_Var
  %lncRs = bitcast i8* @ghczmprim_GHCziCString_unpackCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncRt = load i64*, i64** %Sp_Var
  %lncRu = load i64, i64* %R1_Var
  %lncRv = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncRs( i64* %Base_Arg, i64* %lncRt, i64* %Hp_Arg, i64 %lncRu, i64 %lncRv, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccQZ:
  %lncRx = load i64, i64* %R1_Var
  %lncRy = inttoptr i64 %lncRx to i64*
  %lncRz = load i64, i64* %lncRy, !tbaa !3
  %lncRA = inttoptr i64 %lncRz to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncRB = load i64*, i64** %Sp_Var
  %lncRC = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncRA( i64* %Base_Arg, i64* %lncRB, i64* %Hp_Arg, i64 %lncRC, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccR2:
  %lncRD = load i64, i64* %R1_Var
  store i64 %lncRD, i64* %R1_Var
  %lncRE = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lncRF = bitcast i64* %lncRE to i64*
  %lncRG = load i64, i64* %lncRF, !tbaa !4
  %lncRH = inttoptr i64 %lncRG to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncRI = load i64*, i64** %Sp_Var
  %lncRJ = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncRH( i64* %Base_Arg, i64* %lncRI, i64* %Hp_Arg, i64 %lncRJ, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%rcNc_closure_struct = type <{i64, i64, i64, i64}>
%ccRR_str_struct = type <{[5 x i8]}>
@rcNc_closure$def = internal global %rcNc_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rcNc_info$def to i64), i64 0, i64 0, i64 0}>
@rcNc_closure = internal alias i8* bitcast (%rcNc_closure_struct* @rcNc_closure$def to i8*)
@ccRR_str$def = internal constant %ccRR_str_struct<{[5 x i8] [i8 109, i8 97, i8 105, i8 110, i8 0]}>
@ccRR_str = internal alias i8* bitcast (%ccRR_str_struct* @ccRR_str$def to i8*)
@rcNc_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rcNc_info$def to i8*)
define internal ghccc void @rcNc_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 22}>
{
ccRS:
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
  %lccRO = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lncRV = load i64*, i64** %Sp_Var
  %lncRW = getelementptr inbounds i64, i64* %lncRV, i32 -2
  %lncRX = ptrtoint i64* %lncRW to i64
  %lncRY = icmp ult i64 %lncRX, %SpLim_Arg
  br i1 %lncRY, label %ccRT, label %ccRU
ccRU:
  %lncRZ = ptrtoint i64* %Base_Arg to i64
  %lncS0 = inttoptr i64 %lncRZ to i8*
  %lncS1 = load i64, i64* %R1_Var
  %lncS2 = inttoptr i64 %lncS1 to i8*
  %lncS3 = bitcast i8* @newCAF to i8* (i8*, i8*)*
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
  %lncS4 = call ccc i8* (i8*, i8*) %lncS3( i8* %lncS0, i8* %lncS2 ) nounwind
  %lncS5 = ptrtoint i8* %lncS4 to i64
  store i64 %lncS5, i64* %lccRO
  %lncS6 = load i64, i64* %lccRO
  %lncS7 = icmp eq i64 %lncS6, 0
  br i1 %lncS7, label %ccRQ, label %ccRP
ccRP:
  %lncS9 = ptrtoint i8* @stg_bh_upd_frame_info to i64
  %lncS8 = load i64*, i64** %Sp_Var
  %lncSa = getelementptr inbounds i64, i64* %lncS8, i32 -2
  store i64 %lncS9, i64* %lncSa, !tbaa !1
  %lncSc = load i64, i64* %lccRO
  %lncSb = load i64*, i64** %Sp_Var
  %lncSd = getelementptr inbounds i64, i64* %lncSb, i32 -1
  store i64 %lncSc, i64* %lncSd, !tbaa !1
  %lncSe = ptrtoint %ccRR_str_struct* @ccRR_str$def to i64
  store i64 %lncSe, i64* %R2_Var
  %lncSf = load i64*, i64** %Sp_Var
  %lncSg = getelementptr inbounds i64, i64* %lncSf, i32 -2
  %lncSh = ptrtoint i64* %lncSg to i64
  %lncSi = inttoptr i64 %lncSh to i64*
  store i64* %lncSi, i64** %Sp_Var
  %lncSj = bitcast i8* @ghczmprim_GHCziCString_unpackCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncSk = load i64*, i64** %Sp_Var
  %lncSl = load i64, i64* %R1_Var
  %lncSm = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncSj( i64* %Base_Arg, i64* %lncSk, i64* %Hp_Arg, i64 %lncSl, i64 %lncSm, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccRQ:
  %lncSo = load i64, i64* %R1_Var
  %lncSp = inttoptr i64 %lncSo to i64*
  %lncSq = load i64, i64* %lncSp, !tbaa !3
  %lncSr = inttoptr i64 %lncSq to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncSs = load i64*, i64** %Sp_Var
  %lncSt = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncSr( i64* %Base_Arg, i64* %lncSs, i64* %Hp_Arg, i64 %lncSt, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccRT:
  %lncSu = load i64, i64* %R1_Var
  store i64 %lncSu, i64* %R1_Var
  %lncSv = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lncSw = bitcast i64* %lncSv to i64*
  %lncSx = load i64, i64* %lncSw, !tbaa !4
  %lncSy = inttoptr i64 %lncSx to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncSz = load i64*, i64** %Sp_Var
  %lncSA = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncSy( i64* %Base_Arg, i64* %lncSz, i64* %Hp_Arg, i64 %lncSA, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%rcNd_closure_struct = type <{i64, i64, i64, i64}>
%ccSI_str_struct = type <{[5 x i8]}>
@rcNd_closure$def = internal global %rcNd_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rcNd_info$def to i64), i64 0, i64 0, i64 0}>
@rcNd_closure = internal alias i8* bitcast (%rcNd_closure_struct* @rcNd_closure$def to i8*)
@ccSI_str$def = internal constant %ccSI_str_struct<{[5 x i8] [i8 77, i8 97, i8 105, i8 110, i8 0]}>
@ccSI_str = internal alias i8* bitcast (%ccSI_str_struct* @ccSI_str$def to i8*)
@rcNd_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rcNd_info$def to i8*)
define internal ghccc void @rcNd_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 22}>
{
ccSJ:
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
  %lccSF = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lncSM = load i64*, i64** %Sp_Var
  %lncSN = getelementptr inbounds i64, i64* %lncSM, i32 -2
  %lncSO = ptrtoint i64* %lncSN to i64
  %lncSP = icmp ult i64 %lncSO, %SpLim_Arg
  br i1 %lncSP, label %ccSK, label %ccSL
ccSL:
  %lncSQ = ptrtoint i64* %Base_Arg to i64
  %lncSR = inttoptr i64 %lncSQ to i8*
  %lncSS = load i64, i64* %R1_Var
  %lncST = inttoptr i64 %lncSS to i8*
  %lncSU = bitcast i8* @newCAF to i8* (i8*, i8*)*
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
  %lncSV = call ccc i8* (i8*, i8*) %lncSU( i8* %lncSR, i8* %lncST ) nounwind
  %lncSW = ptrtoint i8* %lncSV to i64
  store i64 %lncSW, i64* %lccSF
  %lncSX = load i64, i64* %lccSF
  %lncSY = icmp eq i64 %lncSX, 0
  br i1 %lncSY, label %ccSH, label %ccSG
ccSG:
  %lncT0 = ptrtoint i8* @stg_bh_upd_frame_info to i64
  %lncSZ = load i64*, i64** %Sp_Var
  %lncT1 = getelementptr inbounds i64, i64* %lncSZ, i32 -2
  store i64 %lncT0, i64* %lncT1, !tbaa !1
  %lncT3 = load i64, i64* %lccSF
  %lncT2 = load i64*, i64** %Sp_Var
  %lncT4 = getelementptr inbounds i64, i64* %lncT2, i32 -1
  store i64 %lncT3, i64* %lncT4, !tbaa !1
  %lncT5 = ptrtoint %ccSI_str_struct* @ccSI_str$def to i64
  store i64 %lncT5, i64* %R2_Var
  %lncT6 = load i64*, i64** %Sp_Var
  %lncT7 = getelementptr inbounds i64, i64* %lncT6, i32 -2
  %lncT8 = ptrtoint i64* %lncT7 to i64
  %lncT9 = inttoptr i64 %lncT8 to i64*
  store i64* %lncT9, i64** %Sp_Var
  %lncTa = bitcast i8* @ghczmprim_GHCziCString_unpackCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncTb = load i64*, i64** %Sp_Var
  %lncTc = load i64, i64* %R1_Var
  %lncTd = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncTa( i64* %Base_Arg, i64* %lncTb, i64* %Hp_Arg, i64 %lncTc, i64 %lncTd, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccSH:
  %lncTf = load i64, i64* %R1_Var
  %lncTg = inttoptr i64 %lncTf to i64*
  %lncTh = load i64, i64* %lncTg, !tbaa !3
  %lncTi = inttoptr i64 %lncTh to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncTj = load i64*, i64** %Sp_Var
  %lncTk = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncTi( i64* %Base_Arg, i64* %lncTj, i64* %Hp_Arg, i64 %lncTk, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccSK:
  %lncTl = load i64, i64* %R1_Var
  store i64 %lncTl, i64* %R1_Var
  %lncTm = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lncTn = bitcast i64* %lncTm to i64*
  %lncTo = load i64, i64* %lncTn, !tbaa !4
  %lncTp = inttoptr i64 %lncTo to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncTq = load i64*, i64** %Sp_Var
  %lncTr = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncTp( i64* %Base_Arg, i64* %lncTq, i64* %Hp_Arg, i64 %lncTr, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%rcNe_closure_struct = type <{i64, i64, i64, i64}>
%ccTz_str_struct = type <{[8 x i8]}>
@rcNe_closure$def = internal global %rcNe_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rcNe_info$def to i64), i64 0, i64 0, i64 0}>
@rcNe_closure = internal alias i8* bitcast (%rcNe_closure_struct* @rcNe_closure$def to i8*)
@ccTz_str$def = internal constant %ccTz_str_struct<{[8 x i8] [i8 109, i8 107, i8 102, i8 115, i8 46, i8 104, i8 115, i8 0]}>
@ccTz_str = internal alias i8* bitcast (%ccTz_str_struct* @ccTz_str$def to i8*)
@rcNe_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rcNe_info$def to i8*)
define internal ghccc void @rcNe_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 22}>
{
ccTA:
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
  %lccTw = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lncTD = load i64*, i64** %Sp_Var
  %lncTE = getelementptr inbounds i64, i64* %lncTD, i32 -2
  %lncTF = ptrtoint i64* %lncTE to i64
  %lncTG = icmp ult i64 %lncTF, %SpLim_Arg
  br i1 %lncTG, label %ccTB, label %ccTC
ccTC:
  %lncTH = ptrtoint i64* %Base_Arg to i64
  %lncTI = inttoptr i64 %lncTH to i8*
  %lncTJ = load i64, i64* %R1_Var
  %lncTK = inttoptr i64 %lncTJ to i8*
  %lncTL = bitcast i8* @newCAF to i8* (i8*, i8*)*
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
  %lncTM = call ccc i8* (i8*, i8*) %lncTL( i8* %lncTI, i8* %lncTK ) nounwind
  %lncTN = ptrtoint i8* %lncTM to i64
  store i64 %lncTN, i64* %lccTw
  %lncTO = load i64, i64* %lccTw
  %lncTP = icmp eq i64 %lncTO, 0
  br i1 %lncTP, label %ccTy, label %ccTx
ccTx:
  %lncTR = ptrtoint i8* @stg_bh_upd_frame_info to i64
  %lncTQ = load i64*, i64** %Sp_Var
  %lncTS = getelementptr inbounds i64, i64* %lncTQ, i32 -2
  store i64 %lncTR, i64* %lncTS, !tbaa !1
  %lncTU = load i64, i64* %lccTw
  %lncTT = load i64*, i64** %Sp_Var
  %lncTV = getelementptr inbounds i64, i64* %lncTT, i32 -1
  store i64 %lncTU, i64* %lncTV, !tbaa !1
  %lncTW = ptrtoint %ccTz_str_struct* @ccTz_str$def to i64
  store i64 %lncTW, i64* %R2_Var
  %lncTX = load i64*, i64** %Sp_Var
  %lncTY = getelementptr inbounds i64, i64* %lncTX, i32 -2
  %lncTZ = ptrtoint i64* %lncTY to i64
  %lncU0 = inttoptr i64 %lncTZ to i64*
  store i64* %lncU0, i64** %Sp_Var
  %lncU1 = bitcast i8* @ghczmprim_GHCziCString_unpackCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncU2 = load i64*, i64** %Sp_Var
  %lncU3 = load i64, i64* %R1_Var
  %lncU4 = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncU1( i64* %Base_Arg, i64* %lncU2, i64* %Hp_Arg, i64 %lncU3, i64 %lncU4, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccTy:
  %lncU6 = load i64, i64* %R1_Var
  %lncU7 = inttoptr i64 %lncU6 to i64*
  %lncU8 = load i64, i64* %lncU7, !tbaa !3
  %lncU9 = inttoptr i64 %lncU8 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncUa = load i64*, i64** %Sp_Var
  %lncUb = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncU9( i64* %Base_Arg, i64* %lncUa, i64* %Hp_Arg, i64 %lncUb, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccTB:
  %lncUc = load i64, i64* %R1_Var
  store i64 %lncUc, i64* %R1_Var
  %lncUd = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lncUe = bitcast i64* %lncUd to i64*
  %lncUf = load i64, i64* %lncUe, !tbaa !4
  %lncUg = inttoptr i64 %lncUf to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncUh = load i64*, i64** %Sp_Var
  %lncUi = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncUg( i64* %Base_Arg, i64* %lncUh, i64* %Hp_Arg, i64 %lncUi, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%rcNf_closure_struct = type <{i64, i64}>
@rcNf_closure$def = internal global %rcNf_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_Izh_static_info to i64), i64 23}>
@rcNf_closure = internal alias i8* bitcast (%rcNf_closure_struct* @rcNf_closure$def to i8*)
%rcNg_closure_struct = type <{i64, i64}>
@rcNg_closure$def = internal global %rcNg_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_Izh_static_info to i64), i64 24}>
@rcNg_closure = internal alias i8* bitcast (%rcNg_closure_struct* @rcNg_closure$def to i8*)
%rcNh_closure_struct = type <{i64, i64}>
@rcNh_closure$def = internal global %rcNh_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_Izh_static_info to i64), i64 45}>
@rcNh_closure = internal alias i8* bitcast (%rcNh_closure_struct* @rcNh_closure$def to i8*)
%rcNi_closure_struct = type <{i64, i64, i64, i64, i64, i64, i64, i64, i64}>
@rcNi_closure$def = internal global %rcNi_closure_struct<{i64 ptrtoint (i8* @base_GHCziStackziTypes_SrcLoc_static_info to i64), i64 ptrtoint (%rcNc_closure_struct* @rcNc_closure$def to i64), i64 ptrtoint (%rcNd_closure_struct* @rcNd_closure$def to i64), i64 ptrtoint (%rcNe_closure_struct* @rcNe_closure$def to i64), i64 add (i64 ptrtoint (%rcNf_closure_struct* @rcNf_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%rcNg_closure_struct* @rcNg_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%rcNf_closure_struct* @rcNf_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%rcNh_closure_struct* @rcNh_closure$def to i64),i64 1), i64 0}>
@rcNi_closure = internal alias i8* bitcast (%rcNi_closure_struct* @rcNi_closure$def to i8*)
%rcNj_closure_struct = type <{i64, i64, i64, i64, i64}>
@rcNj_closure$def = internal global %rcNj_closure_struct<{i64 ptrtoint (i8* @base_GHCziStackziTypes_PushCallStack_static_info to i64), i64 ptrtoint (%rcNb_closure_struct* @rcNb_closure$def to i64), i64 add (i64 ptrtoint (%rcNi_closure_struct* @rcNi_closure$def to i64),i64 1), i64 add (i64 ptrtoint (i8* @base_GHCziStackziTypes_EmptyCallStack_closure to i64),i64 1), i64 0}>
@rcNj_closure = internal alias i8* bitcast (%rcNj_closure_struct* @rcNj_closure$def to i8*)
%rcNk_closure_struct = type <{i64, i64, i64, i64}>
%ccUv_str_struct = type <{[12 x i8]}>
@rcNk_closure$def = internal global %rcNk_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rcNk_info$def to i64), i64 0, i64 0, i64 0}>
@rcNk_closure = internal alias i8* bitcast (%rcNk_closure_struct* @rcNk_closure$def to i8*)
@ccUv_str$def = internal constant %ccUv_str_struct<{[12 x i8] [i8 109, i8 107, i8 102, i8 115, i8 32, i8 102, i8 97, i8 105, i8 108, i8 101, i8 100, i8 0]}>
@ccUv_str = internal alias i8* bitcast (%ccUv_str_struct* @ccUv_str$def to i8*)
@rcNk_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rcNk_info$def to i8*)
define internal ghccc void @rcNk_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 22}>
{
ccUw:
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
  %lccUs = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lncUz = load i64*, i64** %Sp_Var
  %lncUA = getelementptr inbounds i64, i64* %lncUz, i32 -2
  %lncUB = ptrtoint i64* %lncUA to i64
  %lncUC = icmp ult i64 %lncUB, %SpLim_Arg
  br i1 %lncUC, label %ccUx, label %ccUy
ccUy:
  %lncUD = ptrtoint i64* %Base_Arg to i64
  %lncUE = inttoptr i64 %lncUD to i8*
  %lncUF = load i64, i64* %R1_Var
  %lncUG = inttoptr i64 %lncUF to i8*
  %lncUH = bitcast i8* @newCAF to i8* (i8*, i8*)*
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
  %lncUI = call ccc i8* (i8*, i8*) %lncUH( i8* %lncUE, i8* %lncUG ) nounwind
  %lncUJ = ptrtoint i8* %lncUI to i64
  store i64 %lncUJ, i64* %lccUs
  %lncUK = load i64, i64* %lccUs
  %lncUL = icmp eq i64 %lncUK, 0
  br i1 %lncUL, label %ccUu, label %ccUt
ccUt:
  %lncUN = ptrtoint i8* @stg_bh_upd_frame_info to i64
  %lncUM = load i64*, i64** %Sp_Var
  %lncUO = getelementptr inbounds i64, i64* %lncUM, i32 -2
  store i64 %lncUN, i64* %lncUO, !tbaa !1
  %lncUQ = load i64, i64* %lccUs
  %lncUP = load i64*, i64** %Sp_Var
  %lncUR = getelementptr inbounds i64, i64* %lncUP, i32 -1
  store i64 %lncUQ, i64* %lncUR, !tbaa !1
  %lncUS = ptrtoint %ccUv_str_struct* @ccUv_str$def to i64
  store i64 %lncUS, i64* %R2_Var
  %lncUT = load i64*, i64** %Sp_Var
  %lncUU = getelementptr inbounds i64, i64* %lncUT, i32 -2
  %lncUV = ptrtoint i64* %lncUU to i64
  %lncUW = inttoptr i64 %lncUV to i64*
  store i64* %lncUW, i64** %Sp_Var
  %lncUX = bitcast i8* @ghczmprim_GHCziCString_unpackCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncUY = load i64*, i64** %Sp_Var
  %lncUZ = load i64, i64* %R1_Var
  %lncV0 = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncUX( i64* %Base_Arg, i64* %lncUY, i64* %Hp_Arg, i64 %lncUZ, i64 %lncV0, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccUu:
  %lncV2 = load i64, i64* %R1_Var
  %lncV3 = inttoptr i64 %lncV2 to i64*
  %lncV4 = load i64, i64* %lncV3, !tbaa !3
  %lncV5 = inttoptr i64 %lncV4 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncV6 = load i64*, i64** %Sp_Var
  %lncV7 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncV5( i64* %Base_Arg, i64* %lncV6, i64* %Hp_Arg, i64 %lncV7, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccUx:
  %lncV8 = load i64, i64* %R1_Var
  store i64 %lncV8, i64* %R1_Var
  %lncV9 = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lncVa = bitcast i64* %lncV9 to i64*
  %lncVb = load i64, i64* %lncVa, !tbaa !4
  %lncVc = inttoptr i64 %lncVb to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncVd = load i64*, i64** %Sp_Var
  %lncVe = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncVc( i64* %Base_Arg, i64* %lncVd, i64* %Hp_Arg, i64 %lncVe, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%rcNl_closure_struct = type <{i64, i64}>
@rcNl_closure$def = internal global %rcNl_closure_struct<{i64 ptrtoint (i8* @integerzmgmp_GHCziIntegerziType_Szh_static_info to i64), i64 4}>
@rcNl_closure = internal alias i8* bitcast (%rcNl_closure_struct* @rcNl_closure$def to i8*)
%rcNm_closure_struct = type <{i64, i64}>
@rcNm_closure$def = internal global %rcNm_closure_struct<{i64 ptrtoint (i8* @integerzmgmp_GHCziIntegerziType_Szh_static_info to i64), i64 1}>
@rcNm_closure = internal alias i8* bitcast (%rcNm_closure_struct* @rcNm_closure$def to i8*)
%rcNn_closure_struct = type <{i64, i64}>
@rcNn_closure$def = internal global %rcNn_closure_struct<{i64 ptrtoint (i8* @integerzmgmp_GHCziIntegerziType_Szh_static_info to i64), i64 256}>
@rcNn_closure = internal alias i8* bitcast (%rcNn_closure_struct* @rcNn_closure$def to i8*)
%rcNo_closure_struct = type <{i64, i64, i64, i64}>
%ccVp_str_struct = type <{[25 x i8]}>
@rcNo_closure$def = internal global %rcNo_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rcNo_info$def to i64), i64 0, i64 0, i64 0}>
@rcNo_closure = internal alias i8* bitcast (%rcNo_closure_struct* @rcNo_closure$def to i8*)
@ccVp_str$def = internal constant %ccVp_str_struct<{[25 x i8] [i8 73, i8 110, i8 105, i8 116, i8 105, i8 97, i8 108, i8 105, i8 122, i8 105, i8 110, i8 103, i8 32, i8 102, i8 105, i8 108, i8 101, i8 32, i8 115, i8 121, i8 115, i8 116, i8 101, i8 109, i8 0]}>
@ccVp_str = internal alias i8* bitcast (%ccVp_str_struct* @ccVp_str$def to i8*)
@rcNo_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rcNo_info$def to i8*)
define internal ghccc void @rcNo_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 22}>
{
ccVq:
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
  %lccVm = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lncVt = load i64*, i64** %Sp_Var
  %lncVu = getelementptr inbounds i64, i64* %lncVt, i32 -2
  %lncVv = ptrtoint i64* %lncVu to i64
  %lncVw = icmp ult i64 %lncVv, %SpLim_Arg
  br i1 %lncVw, label %ccVr, label %ccVs
ccVs:
  %lncVx = ptrtoint i64* %Base_Arg to i64
  %lncVy = inttoptr i64 %lncVx to i8*
  %lncVz = load i64, i64* %R1_Var
  %lncVA = inttoptr i64 %lncVz to i8*
  %lncVB = bitcast i8* @newCAF to i8* (i8*, i8*)*
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
  %lncVC = call ccc i8* (i8*, i8*) %lncVB( i8* %lncVy, i8* %lncVA ) nounwind
  %lncVD = ptrtoint i8* %lncVC to i64
  store i64 %lncVD, i64* %lccVm
  %lncVE = load i64, i64* %lccVm
  %lncVF = icmp eq i64 %lncVE, 0
  br i1 %lncVF, label %ccVo, label %ccVn
ccVn:
  %lncVH = ptrtoint i8* @stg_bh_upd_frame_info to i64
  %lncVG = load i64*, i64** %Sp_Var
  %lncVI = getelementptr inbounds i64, i64* %lncVG, i32 -2
  store i64 %lncVH, i64* %lncVI, !tbaa !1
  %lncVK = load i64, i64* %lccVm
  %lncVJ = load i64*, i64** %Sp_Var
  %lncVL = getelementptr inbounds i64, i64* %lncVJ, i32 -1
  store i64 %lncVK, i64* %lncVL, !tbaa !1
  %lncVM = ptrtoint %ccVp_str_struct* @ccVp_str$def to i64
  store i64 %lncVM, i64* %R2_Var
  %lncVN = load i64*, i64** %Sp_Var
  %lncVO = getelementptr inbounds i64, i64* %lncVN, i32 -2
  %lncVP = ptrtoint i64* %lncVO to i64
  %lncVQ = inttoptr i64 %lncVP to i64*
  store i64* %lncVQ, i64** %Sp_Var
  %lncVR = bitcast i8* @ghczmprim_GHCziCString_unpackCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncVS = load i64*, i64** %Sp_Var
  %lncVT = load i64, i64* %R1_Var
  %lncVU = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncVR( i64* %Base_Arg, i64* %lncVS, i64* %Hp_Arg, i64 %lncVT, i64 %lncVU, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccVo:
  %lncVW = load i64, i64* %R1_Var
  %lncVX = inttoptr i64 %lncVW to i64*
  %lncVY = load i64, i64* %lncVX, !tbaa !3
  %lncVZ = inttoptr i64 %lncVY to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncW0 = load i64*, i64** %Sp_Var
  %lncW1 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncVZ( i64* %Base_Arg, i64* %lncW0, i64* %Hp_Arg, i64 %lncW1, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccVr:
  %lncW2 = load i64, i64* %R1_Var
  store i64 %lncW2, i64* %R1_Var
  %lncW3 = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lncW4 = bitcast i64* %lncW3 to i64*
  %lncW5 = load i64, i64* %lncW4, !tbaa !4
  %lncW6 = inttoptr i64 %lncW5 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lncW7 = load i64*, i64** %Sp_Var
  %lncW8 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lncW6( i64* %Base_Arg, i64* %lncW7, i64* %Hp_Arg, i64 %lncW8, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Main_main1_closure_struct = type <{i64, i64}>
%ccY8_str_struct = type <{[20 x i8]}>
@Main_main1_closure$def = internal global %Main_main1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Main_main1_info$def to i64), i64 0}>
@Main_main1_closure = alias i8* bitcast (%Main_main1_closure_struct* @Main_main1_closure$def to i8*)
@ccY8_str$def = internal constant %ccY8_str_struct<{[20 x i8] [i8 73, i8 110, i8 105, i8 116, i8 105, i8 97, i8 108, i8 105, i8 122, i8 97, i8 116, i8 105, i8 111, i8 110, i8 32, i8 79, i8 75, i8 44, i8 32, i8 0]}>
@ccY8_str = internal alias i8* bitcast (%ccY8_str_struct* @ccY8_str$def to i8*)
@scNY_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @scNY_info$def to i8*)
define internal ghccc void @scNY_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 3, i64 16}>
{
ccXn:
  %lscNO = alloca i64, i32 1
  %lscNR = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lncZo = load i64*, i64** %Sp_Var
  %lncZp = getelementptr inbounds i64, i64* %lncZo, i32 -5
  %lncZq = ptrtoint i64* %lncZp to i64
  %lncZr = icmp ult i64 %lncZq, %SpLim_Arg
  br i1 %lncZr, label %ccXr, label %ccXs
ccXs:
  %lncZt = ptrtoint i8* @stg_upd_frame_info to i64
  %lncZs = load i64*, i64** %Sp_Var
  %lncZu = getelementptr inbounds i64, i64* %lncZs, i32 -2
  store i64 %lncZt, i64* %lncZu, !tbaa !1
  %lncZw = load i64, i64* %R1_Var
  %lncZv = load i64*, i64** %Sp_Var
  %lncZx = getelementptr inbounds i64, i64* %lncZv, i32 -1
  store i64 %lncZw, i64* %lncZx, !tbaa !1
  %lncZz = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXk_info$def to i64
  %lncZy = load i64*, i64** %Sp_Var
  %lncZA = getelementptr inbounds i64, i64* %lncZy, i32 -5
  store i64 %lncZz, i64* %lncZA, !tbaa !1
  %lncZD = load i64, i64* %R1_Var
  %lncZE = add i64 %lncZD, 24
  %lncZF = inttoptr i64 %lncZE to i64*
  %lncZG = load i64, i64* %lncZF, !tbaa !3
  store i64 %lncZG, i64* %lscNO
  %lncZJ = load i64, i64* %R1_Var
  %lncZK = add i64 %lncZJ, 32
  %lncZL = inttoptr i64 %lncZK to i64*
  %lncZM = load i64, i64* %lncZL, !tbaa !3
  store i64 %lncZM, i64* %lscNR
  %lncZP = load i64, i64* %R1_Var
  %lncZQ = add i64 %lncZP, 16
  %lncZR = inttoptr i64 %lncZQ to i64*
  %lncZS = load i64, i64* %lncZR, !tbaa !3
  store i64 %lncZS, i64* %R1_Var
  %lncZU = load i64, i64* %lscNO
  %lncZT = load i64*, i64** %Sp_Var
  %lncZV = getelementptr inbounds i64, i64* %lncZT, i32 -4
  store i64 %lncZU, i64* %lncZV, !tbaa !1
  %lncZX = load i64, i64* %lscNR
  %lncZW = load i64*, i64** %Sp_Var
  %lncZY = getelementptr inbounds i64, i64* %lncZW, i32 -3
  store i64 %lncZX, i64* %lncZY, !tbaa !1
  %lncZZ = load i64*, i64** %Sp_Var
  %lnd00 = getelementptr inbounds i64, i64* %lncZZ, i32 -5
  %lnd01 = ptrtoint i64* %lnd00 to i64
  %lnd02 = inttoptr i64 %lnd01 to i64*
  store i64* %lnd02, i64** %Sp_Var
  %lnd03 = load i64, i64* %R1_Var
  %lnd04 = and i64 %lnd03, 7
  %lnd05 = icmp ne i64 %lnd04, 0
  br i1 %lnd05, label %ucXw, label %ccXl
ccXl:
  %lnd07 = load i64, i64* %R1_Var
  %lnd08 = inttoptr i64 %lnd07 to i64*
  %lnd09 = load i64, i64* %lnd08, !tbaa !3
  %lnd0a = inttoptr i64 %lnd09 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd0b = load i64*, i64** %Sp_Var
  %lnd0c = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd0a( i64* %Base_Arg, i64* %lnd0b, i64* %Hp_Arg, i64 %lnd0c, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucXw:
  %lnd0d = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXk_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd0e = load i64*, i64** %Sp_Var
  %lnd0f = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd0d( i64* %Base_Arg, i64* %lnd0e, i64* %Hp_Arg, i64 %lnd0f, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccXr:
  %lnd0g = load i64, i64* %R1_Var
  store i64 %lnd0g, i64* %R1_Var
  %lnd0h = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lnd0i = bitcast i64* %lnd0h to i64*
  %lnd0j = load i64, i64* %lnd0i, !tbaa !4
  %lnd0k = inttoptr i64 %lnd0j to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd0l = load i64*, i64** %Sp_Var
  %lnd0m = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd0k( i64* %Base_Arg, i64* %lnd0l, i64* %Hp_Arg, i64 %lnd0m, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccXk_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXk_info$def to i8*)
define internal ghccc void @ccXk_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 2, i64 32}>
{
ccXk:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lscO0 = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnd0n = load i64*, i64** %Hp_Var
  %lnd0o = getelementptr inbounds i64, i64* %lnd0n, i32 5
  %lnd0p = ptrtoint i64* %lnd0o to i64
  %lnd0q = inttoptr i64 %lnd0p to i64*
  store i64* %lnd0q, i64** %Hp_Var
  %lnd0r = load i64*, i64** %Hp_Var
  %lnd0s = ptrtoint i64* %lnd0r to i64
  %lnd0t = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnd0u = bitcast i64* %lnd0t to i64*
  %lnd0v = load i64, i64* %lnd0u, !tbaa !4
  %lnd0w = icmp ugt i64 %lnd0s, %lnd0v
  br i1 %lnd0w, label %ccXv, label %ccXu
ccXu:
  %lnd0z = load i64, i64* %R1_Var
  %lnd0A = add i64 %lnd0z, 7
  %lnd0B = inttoptr i64 %lnd0A to i64*
  %lnd0C = load i64, i64* %lnd0B, !tbaa !3
  store i64 %lnd0C, i64* %lscO0
  %lnd0E = ptrtoint i8* @Disk_S_con_info to i64
  %lnd0D = load i64*, i64** %Hp_Var
  %lnd0F = getelementptr inbounds i64, i64* %lnd0D, i32 -4
  store i64 %lnd0E, i64* %lnd0F, !tbaa !2
  %lnd0H = load i64*, i64** %Sp_Var
  %lnd0I = getelementptr inbounds i64, i64* %lnd0H, i32 1
  %lnd0J = bitcast i64* %lnd0I to i64*
  %lnd0K = load i64, i64* %lnd0J, !tbaa !1
  %lnd0G = load i64*, i64** %Hp_Var
  %lnd0L = getelementptr inbounds i64, i64* %lnd0G, i32 -3
  store i64 %lnd0K, i64* %lnd0L, !tbaa !2
  %lnd0N = ptrtoint i8* @base_GHCziBase_Nothing_closure to i64
  %lnd0O = add i64 %lnd0N, 1
  %lnd0M = load i64*, i64** %Hp_Var
  %lnd0P = getelementptr inbounds i64, i64* %lnd0M, i32 -2
  store i64 %lnd0O, i64* %lnd0P, !tbaa !2
  %lnd0R = load i64*, i64** %Sp_Var
  %lnd0S = getelementptr inbounds i64, i64* %lnd0R, i32 2
  %lnd0T = bitcast i64* %lnd0S to i64*
  %lnd0U = load i64, i64* %lnd0T, !tbaa !1
  %lnd0Q = load i64*, i64** %Hp_Var
  %lnd0V = getelementptr inbounds i64, i64* %lnd0Q, i32 -1
  store i64 %lnd0U, i64* %lnd0V, !tbaa !2
  %lnd0X = load i64, i64* %lscO0
  %lnd0W = load i64*, i64** %Hp_Var
  %lnd0Y = getelementptr inbounds i64, i64* %lnd0W, i32 0
  store i64 %lnd0X, i64* %lnd0Y, !tbaa !2
  %lnd10 = load i64*, i64** %Hp_Var
  %lnd11 = ptrtoint i64* %lnd10 to i64
  %lnd12 = add i64 %lnd11, -31
  store i64 %lnd12, i64* %R1_Var
  %lnd13 = load i64*, i64** %Sp_Var
  %lnd14 = getelementptr inbounds i64, i64* %lnd13, i32 3
  %lnd15 = ptrtoint i64* %lnd14 to i64
  %lnd16 = inttoptr i64 %lnd15 to i64*
  store i64* %lnd16, i64** %Sp_Var
  %lnd17 = load i64*, i64** %Sp_Var
  %lnd18 = getelementptr inbounds i64, i64* %lnd17, i32 0
  %lnd19 = bitcast i64* %lnd18 to i64*
  %lnd1a = load i64, i64* %lnd19, !tbaa !1
  %lnd1b = inttoptr i64 %lnd1a to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd1c = load i64*, i64** %Sp_Var
  %lnd1d = load i64*, i64** %Hp_Var
  %lnd1e = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd1b( i64* %Base_Arg, i64* %lnd1c, i64* %lnd1d, i64 %lnd1e, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccXv:
  %lnd1f = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 40, i64* %lnd1f, !tbaa !4
  %lnd1g = load i64, i64* %R1_Var
  store i64 %lnd1g, i64* %R1_Var
  %lnd1h = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd1i = load i64*, i64** %Sp_Var
  %lnd1j = load i64*, i64** %Hp_Var
  %lnd1k = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd1h( i64* %Base_Arg, i64* %lnd1i, i64* %lnd1j, i64 %lnd1k, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@scOn_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @scOn_info$def to i8*)
define internal ghccc void @scOn_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @scOn_info$def to i64)),i64 24), i64 1, i64 12884901905}>
{
ccXV:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnd1l = load i64*, i64** %Sp_Var
  %lnd1m = getelementptr inbounds i64, i64* %lnd1l, i32 -3
  %lnd1n = ptrtoint i64* %lnd1m to i64
  %lnd1o = icmp ult i64 %lnd1n, %SpLim_Arg
  br i1 %lnd1o, label %ccY1, label %ccY2
ccY2:
  %lnd1q = ptrtoint i8* @stg_upd_frame_info to i64
  %lnd1p = load i64*, i64** %Sp_Var
  %lnd1r = getelementptr inbounds i64, i64* %lnd1p, i32 -2
  store i64 %lnd1q, i64* %lnd1r, !tbaa !1
  %lnd1t = load i64, i64* %R1_Var
  %lnd1s = load i64*, i64** %Sp_Var
  %lnd1u = getelementptr inbounds i64, i64* %lnd1s, i32 -1
  store i64 %lnd1t, i64* %lnd1u, !tbaa !1
  %lnd1w = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXS_info$def to i64
  %lnd1v = load i64*, i64** %Sp_Var
  %lnd1x = getelementptr inbounds i64, i64* %lnd1v, i32 -3
  store i64 %lnd1w, i64* %lnd1x, !tbaa !1
  %lnd1A = load i64, i64* %R1_Var
  %lnd1B = add i64 %lnd1A, 16
  %lnd1C = inttoptr i64 %lnd1B to i64*
  %lnd1D = load i64, i64* %lnd1C, !tbaa !3
  store i64 %lnd1D, i64* %R1_Var
  %lnd1E = load i64*, i64** %Sp_Var
  %lnd1F = getelementptr inbounds i64, i64* %lnd1E, i32 -3
  %lnd1G = ptrtoint i64* %lnd1F to i64
  %lnd1H = inttoptr i64 %lnd1G to i64*
  store i64* %lnd1H, i64** %Sp_Var
  %lnd1I = load i64, i64* %R1_Var
  %lnd1J = and i64 %lnd1I, 7
  %lnd1K = icmp ne i64 %lnd1J, 0
  br i1 %lnd1K, label %ucY7, label %ccXT
ccXT:
  %lnd1M = load i64, i64* %R1_Var
  %lnd1N = inttoptr i64 %lnd1M to i64*
  %lnd1O = load i64, i64* %lnd1N, !tbaa !3
  %lnd1P = inttoptr i64 %lnd1O to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd1Q = load i64*, i64** %Sp_Var
  %lnd1R = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd1P( i64* %Base_Arg, i64* %lnd1Q, i64* %Hp_Arg, i64 %lnd1R, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucY7:
  %lnd1S = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXS_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd1T = load i64*, i64** %Sp_Var
  %lnd1U = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd1S( i64* %Base_Arg, i64* %lnd1T, i64* %Hp_Arg, i64 %lnd1U, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccY1:
  %lnd1V = load i64, i64* %R1_Var
  store i64 %lnd1V, i64* %R1_Var
  %lnd1W = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lnd1X = bitcast i64* %lnd1W to i64*
  %lnd1Y = load i64, i64* %lnd1X, !tbaa !4
  %lnd1Z = inttoptr i64 %lnd1Y to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd20 = load i64*, i64** %Sp_Var
  %lnd21 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd1Z( i64* %Base_Arg, i64* %lnd20, i64* %Hp_Arg, i64 %lnd21, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccXS_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXS_info$def to i8*)
define internal ghccc void @ccXS_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXS_info$def to i64)),i64 24), i64 0, i64 12884901920}>
{
ccXS:
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %lnd22 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXY_info$def to i64
  %lnd23 = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lnd22, i64* %lnd23, !tbaa !1
  %lnd24 = ptrtoint i8* @ghczmprim_GHCziTypes_ZMZN_closure to i64
  %lnd25 = add i64 %lnd24, 1
  store i64 %lnd25, i64* %R4_Var
  %lnd26 = add i64 %R1_Arg, 55
  %lnd27 = inttoptr i64 %lnd26 to i64*
  %lnd28 = load i64, i64* %lnd27, !tbaa !3
  store i64 %lnd28, i64* %R3_Var
  store i64 0, i64* %R2_Var
  %lnd29 = bitcast i8* @base_GHCziShow_zdwzdcshowsPrec3_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd2a = load i64, i64* %R2_Var
  %lnd2b = load i64, i64* %R3_Var
  %lnd2c = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd29( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnd2a, i64 %lnd2b, i64 %lnd2c, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccXY_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXY_info$def to i8*)
define internal ghccc void @ccXY_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXY_info$def to i64)),i64 32), i64 0, i64 4294967328}>
{
ccXY:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnd2d = load i64*, i64** %Hp_Var
  %lnd2e = getelementptr inbounds i64, i64* %lnd2d, i32 3
  %lnd2f = ptrtoint i64* %lnd2e to i64
  %lnd2g = inttoptr i64 %lnd2f to i64*
  store i64* %lnd2g, i64** %Hp_Var
  %lnd2h = load i64*, i64** %Hp_Var
  %lnd2i = ptrtoint i64* %lnd2h to i64
  %lnd2j = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnd2k = bitcast i64* %lnd2j to i64*
  %lnd2l = load i64, i64* %lnd2k, !tbaa !4
  %lnd2m = icmp ugt i64 %lnd2i, %lnd2l
  br i1 %lnd2m, label %ccY6, label %ccY5
ccY5:
  %lnd2o = ptrtoint i8* @ghczmprim_GHCziTypes_ZC_con_info to i64
  %lnd2n = load i64*, i64** %Hp_Var
  %lnd2p = getelementptr inbounds i64, i64* %lnd2n, i32 -2
  store i64 %lnd2o, i64* %lnd2p, !tbaa !2
  %lnd2r = load i64, i64* %R1_Var
  %lnd2q = load i64*, i64** %Hp_Var
  %lnd2s = getelementptr inbounds i64, i64* %lnd2q, i32 -1
  store i64 %lnd2r, i64* %lnd2s, !tbaa !2
  %lnd2u = load i64, i64* %R2_Var
  %lnd2t = load i64*, i64** %Hp_Var
  %lnd2v = getelementptr inbounds i64, i64* %lnd2t, i32 0
  store i64 %lnd2u, i64* %lnd2v, !tbaa !2
  %lnd2w = ptrtoint %rcNa_closure_struct* @rcNa_closure$def to i64
  store i64 %lnd2w, i64* %R3_Var
  %lnd2y = load i64*, i64** %Hp_Var
  %lnd2z = ptrtoint i64* %lnd2y to i64
  %lnd2A = add i64 %lnd2z, -14
  store i64 %lnd2A, i64* %R2_Var
  %lnd2B = load i64*, i64** %Sp_Var
  %lnd2C = getelementptr inbounds i64, i64* %lnd2B, i32 1
  %lnd2D = ptrtoint i64* %lnd2C to i64
  %lnd2E = inttoptr i64 %lnd2D to i64*
  store i64* %lnd2E, i64** %Sp_Var
  %lnd2F = bitcast i8* @base_GHCziBase_zpzp_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd2G = load i64*, i64** %Sp_Var
  %lnd2H = load i64*, i64** %Hp_Var
  %lnd2I = load i64, i64* %R1_Var
  %lnd2J = load i64, i64* %R2_Var
  %lnd2K = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd2F( i64* %Base_Arg, i64* %lnd2G, i64* %lnd2H, i64 %lnd2I, i64 %lnd2J, i64 %lnd2K, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccY6:
  %lnd2L = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lnd2L, !tbaa !4
  %lnd2M = load i64, i64* %R2_Var
  store i64 %lnd2M, i64* %R2_Var
  %lnd2N = load i64, i64* %R1_Var
  store i64 %lnd2N, i64* %R1_Var
  %lnd2O = bitcast i8* @stg_gc_pp to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd2P = load i64*, i64** %Sp_Var
  %lnd2Q = load i64*, i64** %Hp_Var
  %lnd2R = load i64, i64* %R1_Var
  %lnd2S = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd2O( i64* %Base_Arg, i64* %lnd2P, i64* %lnd2Q, i64 %lnd2R, i64 %lnd2S, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@scOo_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @scOo_info$def to i8*)
define internal ghccc void @scOo_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @scOo_info$def to i64)),i64 24), i64 1, i64 12884901905}>
{
ccY9:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lscO9 = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnd2T = load i64*, i64** %Hp_Var
  %lnd2U = getelementptr inbounds i64, i64* %lnd2T, i32 3
  %lnd2V = ptrtoint i64* %lnd2U to i64
  %lnd2W = inttoptr i64 %lnd2V to i64*
  store i64* %lnd2W, i64** %Hp_Var
  %lnd2X = load i64*, i64** %Hp_Var
  %lnd2Y = ptrtoint i64* %lnd2X to i64
  %lnd2Z = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnd30 = bitcast i64* %lnd2Z to i64*
  %lnd31 = load i64, i64* %lnd30, !tbaa !4
  %lnd32 = icmp ugt i64 %lnd2Y, %lnd31
  br i1 %lnd32, label %ccYd, label %ccYc
ccYc:
  %lnd35 = load i64, i64* %R1_Var
  %lnd36 = add i64 %lnd35, 16
  %lnd37 = inttoptr i64 %lnd36 to i64*
  %lnd38 = load i64, i64* %lnd37, !tbaa !3
  store i64 %lnd38, i64* %lscO9
  %lnd3a = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @scOn_info$def to i64
  %lnd39 = load i64*, i64** %Hp_Var
  %lnd3b = getelementptr inbounds i64, i64* %lnd39, i32 -2
  store i64 %lnd3a, i64* %lnd3b, !tbaa !2
  %lnd3d = load i64, i64* %lscO9
  %lnd3c = load i64*, i64** %Hp_Var
  %lnd3e = getelementptr inbounds i64, i64* %lnd3c, i32 0
  store i64 %lnd3d, i64* %lnd3e, !tbaa !2
  %lnd3f = load i64*, i64** %Hp_Var
  %lnd3g = getelementptr inbounds i64, i64* %lnd3f, i32 -2
  %lnd3h = ptrtoint i64* %lnd3g to i64
  store i64 %lnd3h, i64* %R3_Var
  %lnd3i = ptrtoint %ccY8_str_struct* @ccY8_str$def to i64
  store i64 %lnd3i, i64* %R2_Var
  %lnd3j = bitcast i8* @ghczmprim_GHCziCString_unpackAppendCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd3k = load i64*, i64** %Hp_Var
  %lnd3l = load i64, i64* %R1_Var
  %lnd3m = load i64, i64* %R2_Var
  %lnd3n = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd3j( i64* %Base_Arg, i64* %Sp_Arg, i64* %lnd3k, i64 %lnd3l, i64 %lnd3m, i64 %lnd3n, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccYd:
  %lnd3o = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lnd3o, !tbaa !4
  %lnd3p = load i64, i64* %R1_Var
  store i64 %lnd3p, i64* %R1_Var
  %lnd3q = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %lnd3r = bitcast i64* %lnd3q to i64*
  %lnd3s = load i64, i64* %lnd3r, !tbaa !4
  %lnd3t = inttoptr i64 %lnd3s to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd3u = load i64*, i64** %Hp_Var
  %lnd3v = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd3t( i64* %Base_Arg, i64* %Sp_Arg, i64* %lnd3u, i64 %lnd3v, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@Main_main1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Main_main1_info$def to i8*)
define ghccc void @Main_main1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Main_main1_info$def to i64)),i64 0), i64 4294967299, i64 0, i64 1125878432006159}>
{
ccYi:
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnd3w = load i64*, i64** %Sp_Var
  %lnd3x = getelementptr inbounds i64, i64* %lnd3w, i32 -4
  %lnd3y = ptrtoint i64* %lnd3x to i64
  %lnd3z = icmp ult i64 %lnd3y, %SpLim_Arg
  br i1 %lnd3z, label %ccYj, label %ccYk
ccYk:
  %lnd3B = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccWT_info$def to i64
  %lnd3A = load i64*, i64** %Sp_Var
  %lnd3C = getelementptr inbounds i64, i64* %lnd3A, i32 -1
  store i64 %lnd3B, i64* %lnd3C, !tbaa !1
  %lnd3D = load i64*, i64** %Sp_Var
  %lnd3E = getelementptr inbounds i64, i64* %lnd3D, i32 -1
  %lnd3F = ptrtoint i64* %lnd3E to i64
  %lnd3G = inttoptr i64 %lnd3F to i64*
  store i64* %lnd3G, i64** %Sp_Var
  %lnd3H = bitcast i8* @base_SystemziEnvironment_getArgs1_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd3I = load i64*, i64** %Sp_Var
  %lnd3J = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd3H( i64* %Base_Arg, i64* %lnd3I, i64* %Hp_Arg, i64 %lnd3J, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccYj:
  %lnd3K = ptrtoint %Main_main1_closure_struct* @Main_main1_closure$def to i64
  store i64 %lnd3K, i64* %R1_Var
  %lnd3L = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %lnd3M = bitcast i64* %lnd3L to i64*
  %lnd3N = load i64, i64* %lnd3M, !tbaa !4
  %lnd3O = inttoptr i64 %lnd3N to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd3P = load i64*, i64** %Sp_Var
  %lnd3Q = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd3O( i64* %Base_Arg, i64* %lnd3P, i64* %Hp_Arg, i64 %lnd3Q, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccWT_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccWT_info$def to i8*)
define internal ghccc void @ccWT_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccWT_info$def to i64)),i64 0), i64 0, i64 1073101873872928}>
{
ccWT:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnd3R = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccWV_info$def to i64
  %lnd3S = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lnd3R, i64* %lnd3S, !tbaa !1
  %lnd3T = load i64, i64* %R1_Var
  store i64 %lnd3T, i64* %R1_Var
  %lnd3U = load i64, i64* %R1_Var
  %lnd3V = and i64 %lnd3U, 7
  %lnd3W = icmp ne i64 %lnd3V, 0
  br i1 %lnd3W, label %ucZj, label %ccWW
ccWW:
  %lnd3Y = load i64, i64* %R1_Var
  %lnd3Z = inttoptr i64 %lnd3Y to i64*
  %lnd40 = load i64, i64* %lnd3Z, !tbaa !3
  %lnd41 = inttoptr i64 %lnd40 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd42 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd41( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnd42, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucZj:
  %lnd43 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccWV_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd44 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd43( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnd44, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccWV_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccWV_info$def to i8*)
define internal ghccc void @ccWV_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccWV_info$def to i64)),i64 0), i64 0, i64 1073101873872928}>
{
ccWV:
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lscNG = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnd45 = load i64, i64* %R1_Var
  %lnd46 = and i64 %lnd45, 7
  switch i64 %lnd46, label %ccYf [i64 1, label %ccYf
i64 2, label %ccYg]
ccYf:
  %lnd47 = load i64*, i64** %Sp_Var
  %lnd48 = getelementptr inbounds i64, i64* %lnd47, i32 1
  %lnd49 = ptrtoint i64* %lnd48 to i64
  %lnd4a = inttoptr i64 %lnd49 to i64*
  store i64* %lnd4a, i64** %Sp_Var
  %lnd4b = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rcN9_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd4c = load i64*, i64** %Sp_Var
  %lnd4d = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd4b( i64* %Base_Arg, i64* %lnd4c, i64* %Hp_Arg, i64 %lnd4d, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccYg:
  %lnd4f = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccX1_info$def to i64
  %lnd4e = load i64*, i64** %Sp_Var
  %lnd4g = getelementptr inbounds i64, i64* %lnd4e, i32 -1
  store i64 %lnd4f, i64* %lnd4g, !tbaa !1
  %lnd4j = load i64, i64* %R1_Var
  %lnd4k = add i64 %lnd4j, 6
  %lnd4l = inttoptr i64 %lnd4k to i64*
  %lnd4m = load i64, i64* %lnd4l, !tbaa !3
  store i64 %lnd4m, i64* %lscNG
  %lnd4p = load i64, i64* %R1_Var
  %lnd4q = add i64 %lnd4p, 14
  %lnd4r = inttoptr i64 %lnd4q to i64*
  %lnd4s = load i64, i64* %lnd4r, !tbaa !3
  store i64 %lnd4s, i64* %R1_Var
  %lnd4u = load i64, i64* %lscNG
  %lnd4t = load i64*, i64** %Sp_Var
  %lnd4v = getelementptr inbounds i64, i64* %lnd4t, i32 0
  store i64 %lnd4u, i64* %lnd4v, !tbaa !1
  %lnd4w = load i64*, i64** %Sp_Var
  %lnd4x = getelementptr inbounds i64, i64* %lnd4w, i32 -1
  %lnd4y = ptrtoint i64* %lnd4x to i64
  %lnd4z = inttoptr i64 %lnd4y to i64*
  store i64* %lnd4z, i64** %Sp_Var
  %lnd4A = load i64, i64* %R1_Var
  %lnd4B = and i64 %lnd4A, 7
  %lnd4C = icmp ne i64 %lnd4B, 0
  br i1 %lnd4C, label %ucZk, label %ccX2
ccX2:
  %lnd4E = load i64, i64* %R1_Var
  %lnd4F = inttoptr i64 %lnd4E to i64*
  %lnd4G = load i64, i64* %lnd4F, !tbaa !3
  %lnd4H = inttoptr i64 %lnd4G to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd4I = load i64*, i64** %Sp_Var
  %lnd4J = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd4H( i64* %Base_Arg, i64* %lnd4I, i64* %Hp_Arg, i64 %lnd4J, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucZk:
  %lnd4K = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccX1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd4L = load i64*, i64** %Sp_Var
  %lnd4M = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd4K( i64* %Base_Arg, i64* %lnd4L, i64* %Hp_Arg, i64 %lnd4M, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccX1_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccX1_info$def to i8*)
define internal ghccc void @ccX1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccX1_info$def to i64)),i64 0), i64 1, i64 1073101873872928}>
{
ccX1:
  %lscNG = alloca i64, i32 1
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
  %lnd4N = and i64 %R1_Arg, 7
  switch i64 %lnd4N, label %ccYr [i64 1, label %ccYr
i64 2, label %ccZg]
ccYr:
  %lnd4O = load i64*, i64** %Sp_Var
  %lnd4P = getelementptr inbounds i64, i64* %lnd4O, i32 1
  %lnd4Q = bitcast i64* %lnd4P to i64*
  %lnd4R = load i64, i64* %lnd4Q, !tbaa !1
  store i64 %lnd4R, i64* %lscNG
  %lnd4T = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccX6_info$def to i64
  %lnd4S = load i64*, i64** %Sp_Var
  %lnd4U = getelementptr inbounds i64, i64* %lnd4S, i32 1
  store i64 %lnd4T, i64* %lnd4U, !tbaa !1
  %lnd4V = ptrtoint i8* @unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_defaultFileFlags_closure to i64
  store i64 %lnd4V, i64* %R5_Var
  %lnd4W = ptrtoint i8* @Disk_initzudisk4_closure to i64
  store i64 %lnd4W, i64* %R4_Var
  %lnd4X = ptrtoint i8* @unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_ReadWrite_closure to i64
  %lnd4Y = add i64 %lnd4X, 3
  store i64 %lnd4Y, i64* %R3_Var
  %lnd4Z = load i64, i64* %lscNG
  store i64 %lnd4Z, i64* %R2_Var
  %lnd50 = load i64*, i64** %Sp_Var
  %lnd51 = getelementptr inbounds i64, i64* %lnd50, i32 1
  %lnd52 = ptrtoint i64* %lnd51 to i64
  %lnd53 = inttoptr i64 %lnd52 to i64*
  store i64* %lnd53, i64** %Sp_Var
  %lnd54 = bitcast i8* @unixzm2zi7zi2zi0_SystemziPosixziIO_createFile3_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd55 = load i64*, i64** %Sp_Var
  %lnd56 = load i64, i64* %R2_Var
  %lnd57 = load i64, i64* %R3_Var
  %lnd58 = load i64, i64* %R4_Var
  %lnd59 = load i64, i64* %R5_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd54( i64* %Base_Arg, i64* %lnd55, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnd56, i64 %lnd57, i64 %lnd58, i64 %lnd59, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccZg:
  %lnd5a = load i64*, i64** %Sp_Var
  %lnd5b = getelementptr inbounds i64, i64* %lnd5a, i32 2
  %lnd5c = ptrtoint i64* %lnd5b to i64
  %lnd5d = inttoptr i64 %lnd5c to i64*
  store i64* %lnd5d, i64** %Sp_Var
  %lnd5e = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @rcN9_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd5f = load i64*, i64** %Sp_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd5e( i64* %Base_Arg, i64* %lnd5f, i64* %Hp_Arg, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccX6_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccX6_info$def to i8*)
define internal ghccc void @ccX6_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccX6_info$def to i64)),i64 0), i64 0, i64 1002183373881376}>
{
ccX6:
  %lscNL = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnd5h = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccX8_info$def to i64
  %lnd5g = load i64*, i64** %Sp_Var
  %lnd5i = getelementptr inbounds i64, i64* %lnd5g, i32 -1
  store i64 %lnd5h, i64* %lnd5i, !tbaa !1
  %lnd5j = load i64, i64* %R1_Var
  store i64 %lnd5j, i64* %lscNL
  %lnd5k = ptrtoint i8* @Disk_clearzustats2_closure to i64
  store i64 %lnd5k, i64* %R1_Var
  %lnd5m = load i64, i64* %lscNL
  %lnd5l = load i64*, i64** %Sp_Var
  %lnd5n = getelementptr inbounds i64, i64* %lnd5l, i32 0
  store i64 %lnd5m, i64* %lnd5n, !tbaa !1
  %lnd5o = load i64*, i64** %Sp_Var
  %lnd5p = getelementptr inbounds i64, i64* %lnd5o, i32 -1
  %lnd5q = ptrtoint i64* %lnd5p to i64
  %lnd5r = inttoptr i64 %lnd5q to i64*
  store i64* %lnd5r, i64** %Sp_Var
  %lnd5s = bitcast i8* @stg_newMutVarzh to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd5t = load i64*, i64** %Sp_Var
  %lnd5u = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd5s( i64* %Base_Arg, i64* %lnd5t, i64* %Hp_Arg, i64 %lnd5u, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccX8_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccX8_info$def to i8*)
define internal ghccc void @ccX8_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccX8_info$def to i64)),i64 0), i64 1, i64 1002183373881376}>
{
ccX8:
  %lscNO = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnd5w = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXa_info$def to i64
  %lnd5v = load i64*, i64** %Sp_Var
  %lnd5x = getelementptr inbounds i64, i64* %lnd5v, i32 -1
  store i64 %lnd5w, i64* %lnd5x, !tbaa !1
  %lnd5y = load i64, i64* %R1_Var
  store i64 %lnd5y, i64* %lscNO
  %lnd5z = ptrtoint i8* @Disk_initzudisk2_closure to i64
  store i64 %lnd5z, i64* %R1_Var
  %lnd5B = load i64, i64* %lscNO
  %lnd5A = load i64*, i64** %Sp_Var
  %lnd5C = getelementptr inbounds i64, i64* %lnd5A, i32 0
  store i64 %lnd5B, i64* %lnd5C, !tbaa !1
  %lnd5D = load i64*, i64** %Sp_Var
  %lnd5E = getelementptr inbounds i64, i64* %lnd5D, i32 -1
  %lnd5F = ptrtoint i64* %lnd5E to i64
  %lnd5G = inttoptr i64 %lnd5F to i64*
  store i64* %lnd5G, i64** %Sp_Var
  %lnd5H = bitcast i8* @stg_newMutVarzh to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd5I = load i64*, i64** %Sp_Var
  %lnd5J = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd5H( i64* %Base_Arg, i64* %lnd5I, i64* %Hp_Arg, i64 %lnd5J, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccXa_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXa_info$def to i8*)
define internal ghccc void @ccXa_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXa_info$def to i64)),i64 0), i64 2, i64 1002183373881376}>
{
ccXa:
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnd5L = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXc_info$def to i64
  %lnd5K = load i64*, i64** %Sp_Var
  %lnd5M = getelementptr inbounds i64, i64* %lnd5K, i32 -1
  store i64 %lnd5L, i64* %lnd5M, !tbaa !1
  %lnd5N = ptrtoint i8* @ghczmprim_GHCziTypes_True_closure to i64
  %lnd5O = add i64 %lnd5N, 2
  store i64 %lnd5O, i64* %R4_Var
  %lnd5P = ptrtoint %rcNo_closure_struct* @rcNo_closure$def to i64
  store i64 %lnd5P, i64* %R3_Var
  %lnd5Q = ptrtoint i8* @base_GHCziIOziHandleziFD_stdout_closure to i64
  store i64 %lnd5Q, i64* %R2_Var
  %lnd5R = load i64*, i64** %Sp_Var
  %lnd5S = getelementptr inbounds i64, i64* %lnd5R, i32 0
  store i64 %R1_Arg, i64* %lnd5S, !tbaa !1
  %lnd5T = load i64*, i64** %Sp_Var
  %lnd5U = getelementptr inbounds i64, i64* %lnd5T, i32 -1
  %lnd5V = ptrtoint i64* %lnd5U to i64
  %lnd5W = inttoptr i64 %lnd5V to i64*
  store i64* %lnd5W, i64** %Sp_Var
  %lnd5X = bitcast i8* @base_GHCziIOziHandleziText_hPutStr2_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd5Y = load i64*, i64** %Sp_Var
  %lnd5Z = load i64, i64* %R2_Var
  %lnd60 = load i64, i64* %R3_Var
  %lnd61 = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd5X( i64* %Base_Arg, i64* %lnd5Y, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnd5Z, i64 %lnd60, i64 %lnd61, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccXc_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXc_info$def to i8*)
define internal ghccc void @ccXc_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXc_info$def to i64)),i64 0), i64 3, i64 439233420460064}>
{
ccXc:
  %R5_Var = alloca i64, i32 1
  store i64 undef, i64* %R5_Var
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %lnd62 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXe_info$def to i64
  %lnd63 = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lnd62, i64* %lnd63, !tbaa !1
  %lnd64 = ptrtoint %rcNn_closure_struct* @rcNn_closure$def to i64
  %lnd65 = add i64 %lnd64, 1
  store i64 %lnd65, i64* %R5_Var
  %lnd66 = ptrtoint %rcNm_closure_struct* @rcNm_closure$def to i64
  %lnd67 = add i64 %lnd66, 1
  store i64 %lnd67, i64* %R4_Var
  %lnd68 = ptrtoint %rcNl_closure_struct* @rcNl_closure$def to i64
  %lnd69 = add i64 %lnd68, 1
  store i64 %lnd69, i64* %R3_Var
  %lnd6a = ptrtoint %Main_cachesizze_closure_struct* @Main_cachesizze_closure$def to i64
  %lnd6b = add i64 %lnd6a, 1
  store i64 %lnd6b, i64* %R2_Var
  %lnd6c = bitcast i8* @AsyncFS_zdwzuAFSzuzumkfs_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd6d = load i64, i64* %R2_Var
  %lnd6e = load i64, i64* %R3_Var
  %lnd6f = load i64, i64* %R4_Var
  %lnd6g = load i64, i64* %R5_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd6c( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnd6d, i64 %lnd6e, i64 %lnd6f, i64 %lnd6g, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccXe_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXe_info$def to i8*)
define internal ghccc void @ccXe_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXe_info$def to i64)),i64 0), i64 3, i64 434835373948960}>
{
ccXe:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %lccXg = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnd6h = load i64*, i64** %Hp_Var
  %lnd6i = getelementptr inbounds i64, i64* %lnd6h, i32 8
  %lnd6j = ptrtoint i64* %lnd6i to i64
  %lnd6k = inttoptr i64 %lnd6j to i64*
  store i64* %lnd6k, i64** %Hp_Var
  %lnd6l = load i64*, i64** %Hp_Var
  %lnd6m = ptrtoint i64* %lnd6l to i64
  %lnd6n = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnd6o = bitcast i64* %lnd6n to i64*
  %lnd6p = load i64, i64* %lnd6o, !tbaa !4
  %lnd6q = icmp ugt i64 %lnd6m, %lnd6p
  br i1 %lnd6q, label %ccYy, label %ccYx
ccYx:
  %lnd6s = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @scNY_info$def to i64
  %lnd6r = load i64*, i64** %Hp_Var
  %lnd6t = getelementptr inbounds i64, i64* %lnd6r, i32 -7
  store i64 %lnd6s, i64* %lnd6t, !tbaa !2
  %lnd6v = load i64*, i64** %Sp_Var
  %lnd6w = getelementptr inbounds i64, i64* %lnd6v, i32 3
  %lnd6x = bitcast i64* %lnd6w to i64*
  %lnd6y = load i64, i64* %lnd6x, !tbaa !1
  %lnd6u = load i64*, i64** %Hp_Var
  %lnd6z = getelementptr inbounds i64, i64* %lnd6u, i32 -5
  store i64 %lnd6y, i64* %lnd6z, !tbaa !2
  %lnd6B = load i64*, i64** %Sp_Var
  %lnd6C = getelementptr inbounds i64, i64* %lnd6B, i32 2
  %lnd6D = bitcast i64* %lnd6C to i64*
  %lnd6E = load i64, i64* %lnd6D, !tbaa !1
  %lnd6A = load i64*, i64** %Hp_Var
  %lnd6F = getelementptr inbounds i64, i64* %lnd6A, i32 -4
  store i64 %lnd6E, i64* %lnd6F, !tbaa !2
  %lnd6H = load i64*, i64** %Sp_Var
  %lnd6I = getelementptr inbounds i64, i64* %lnd6H, i32 1
  %lnd6J = bitcast i64* %lnd6I to i64*
  %lnd6K = load i64, i64* %lnd6J, !tbaa !1
  %lnd6G = load i64*, i64** %Hp_Var
  %lnd6L = getelementptr inbounds i64, i64* %lnd6G, i32 -3
  store i64 %lnd6K, i64* %lnd6L, !tbaa !2
  %lnd6N = ptrtoint i8* @Prog_Bind_con_info to i64
  %lnd6M = load i64*, i64** %Hp_Var
  %lnd6O = getelementptr inbounds i64, i64* %lnd6M, i32 -2
  store i64 %lnd6N, i64* %lnd6O, !tbaa !2
  %lnd6Q = load i64, i64* %R1_Var
  %lnd6P = load i64*, i64** %Hp_Var
  %lnd6R = getelementptr inbounds i64, i64* %lnd6P, i32 -1
  store i64 %lnd6Q, i64* %lnd6R, !tbaa !2
  %lnd6T = load i64, i64* %R2_Var
  %lnd6S = load i64*, i64** %Hp_Var
  %lnd6U = getelementptr inbounds i64, i64* %lnd6S, i32 0
  store i64 %lnd6T, i64* %lnd6U, !tbaa !2
  %lnd6W = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXy_info$def to i64
  %lnd6V = load i64*, i64** %Sp_Var
  %lnd6X = getelementptr inbounds i64, i64* %lnd6V, i32 2
  store i64 %lnd6W, i64* %lnd6X, !tbaa !1
  %lnd6Z = load i64*, i64** %Hp_Var
  %lnd70 = ptrtoint i64* %lnd6Z to i64
  %lnd71 = add i64 %lnd70, -15
  store i64 %lnd71, i64* %R3_Var
  %lnd72 = load i64*, i64** %Hp_Var
  %lnd73 = getelementptr inbounds i64, i64* %lnd72, i32 -7
  %lnd74 = ptrtoint i64* %lnd73 to i64
  store i64 %lnd74, i64* %lccXg
  %lnd75 = load i64, i64* %lccXg
  store i64 %lnd75, i64* %R2_Var
  %lnd77 = load i64, i64* %lccXg
  %lnd76 = load i64*, i64** %Sp_Var
  %lnd78 = getelementptr inbounds i64, i64* %lnd76, i32 3
  store i64 %lnd77, i64* %lnd78, !tbaa !1
  %lnd79 = load i64*, i64** %Sp_Var
  %lnd7a = getelementptr inbounds i64, i64* %lnd79, i32 2
  %lnd7b = ptrtoint i64* %lnd7a to i64
  %lnd7c = inttoptr i64 %lnd7b to i64*
  store i64* %lnd7c, i64** %Sp_Var
  %lnd7d = bitcast i8* @Interpreter_run2_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd7e = load i64*, i64** %Sp_Var
  %lnd7f = load i64*, i64** %Hp_Var
  %lnd7g = load i64, i64* %R1_Var
  %lnd7h = load i64, i64* %R2_Var
  %lnd7i = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd7d( i64* %Base_Arg, i64* %lnd7e, i64* %lnd7f, i64 %lnd7g, i64 %lnd7h, i64 %lnd7i, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccYy:
  %lnd7j = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 64, i64* %lnd7j, !tbaa !4
  %lnd7k = load i64, i64* %R2_Var
  store i64 %lnd7k, i64* %R2_Var
  %lnd7l = load i64, i64* %R1_Var
  store i64 %lnd7l, i64* %R1_Var
  %lnd7m = bitcast i8* @stg_gc_pp to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd7n = load i64*, i64** %Sp_Var
  %lnd7o = load i64*, i64** %Hp_Var
  %lnd7p = load i64, i64* %R1_Var
  %lnd7q = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd7m( i64* %Base_Arg, i64* %lnd7n, i64* %lnd7o, i64 %lnd7p, i64 %lnd7q, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccXy_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXy_info$def to i8*)
define internal ghccc void @ccXy_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXy_info$def to i64)),i64 0), i64 1, i64 426039280926752}>
{
ccXy:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnd7r = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXA_info$def to i64
  %lnd7s = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lnd7r, i64* %lnd7s, !tbaa !1
  %lnd7t = load i64, i64* %R1_Var
  store i64 %lnd7t, i64* %R1_Var
  %lnd7u = load i64, i64* %R1_Var
  %lnd7v = and i64 %lnd7u, 7
  %lnd7w = icmp ne i64 %lnd7v, 0
  br i1 %lnd7w, label %ucZl, label %ccXB
ccXB:
  %lnd7y = load i64, i64* %R1_Var
  %lnd7z = inttoptr i64 %lnd7y to i64*
  %lnd7A = load i64, i64* %lnd7z, !tbaa !3
  %lnd7B = inttoptr i64 %lnd7A to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd7C = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd7B( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnd7C, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucZl:
  %lnd7D = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXA_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd7E = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd7D( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %lnd7E, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccXA_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXA_info$def to i8*)
define internal ghccc void @ccXA_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXA_info$def to i64)),i64 0), i64 1, i64 426039280926752}>
{
ccXA:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnd7F = load i64, i64* %R1_Var
  %lnd7G = and i64 %lnd7F, 7
  switch i64 %lnd7G, label %ccYF [i64 1, label %ccYF
i64 2, label %ccZc]
ccYF:
  %lnd7I = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXF_info$def to i64
  %lnd7H = load i64*, i64** %Sp_Var
  %lnd7J = getelementptr inbounds i64, i64* %lnd7H, i32 0
  store i64 %lnd7I, i64* %lnd7J, !tbaa !1
  %lnd7M = load i64, i64* %R1_Var
  %lnd7N = add i64 %lnd7M, 7
  %lnd7O = inttoptr i64 %lnd7N to i64*
  %lnd7P = load i64, i64* %lnd7O, !tbaa !3
  store i64 %lnd7P, i64* %R1_Var
  %lnd7Q = load i64, i64* %R1_Var
  %lnd7R = and i64 %lnd7Q, 7
  %lnd7S = icmp ne i64 %lnd7R, 0
  br i1 %lnd7S, label %ucZm, label %ccXG
ccXG:
  %lnd7U = load i64, i64* %R1_Var
  %lnd7V = inttoptr i64 %lnd7U to i64*
  %lnd7W = load i64, i64* %lnd7V, !tbaa !3
  %lnd7X = inttoptr i64 %lnd7W to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd7Y = load i64*, i64** %Sp_Var
  %lnd7Z = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd7X( i64* %Base_Arg, i64* %lnd7Y, i64* %Hp_Arg, i64 %lnd7Z, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucZm:
  %lnd80 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXF_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd81 = load i64*, i64** %Sp_Var
  %lnd82 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd80( i64* %Base_Arg, i64* %lnd81, i64* %Hp_Arg, i64 %lnd82, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccZc:
  %lnd83 = ptrtoint %rcNk_closure_struct* @rcNk_closure$def to i64
  store i64 %lnd83, i64* %R3_Var
  %lnd84 = ptrtoint %rcNj_closure_struct* @rcNj_closure$def to i64
  %lnd85 = add i64 %lnd84, 2
  store i64 %lnd85, i64* %R2_Var
  %lnd86 = load i64*, i64** %Sp_Var
  %lnd87 = getelementptr inbounds i64, i64* %lnd86, i32 2
  %lnd88 = ptrtoint i64* %lnd87 to i64
  %lnd89 = inttoptr i64 %lnd88 to i64*
  store i64* %lnd89, i64** %Sp_Var
  %lnd8a = bitcast i8* @base_GHCziErr_error_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd8b = load i64*, i64** %Sp_Var
  %lnd8c = load i64, i64* %R1_Var
  %lnd8d = load i64, i64* %R2_Var
  %lnd8e = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd8a( i64* %Base_Arg, i64* %lnd8b, i64* %Hp_Arg, i64 %lnd8c, i64 %lnd8d, i64 %lnd8e, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccXF_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXF_info$def to i8*)
define internal ghccc void @ccXF_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccXF_info$def to i64)),i64 0), i64 1, i64 3689376907296}>
{
ccXF:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lscO9 = alloca i64, i32 1
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %lnd8f = load i64*, i64** %Hp_Var
  %lnd8g = getelementptr inbounds i64, i64* %lnd8f, i32 3
  %lnd8h = ptrtoint i64* %lnd8g to i64
  %lnd8i = inttoptr i64 %lnd8h to i64*
  store i64* %lnd8i, i64** %Hp_Var
  %lnd8j = load i64*, i64** %Hp_Var
  %lnd8k = ptrtoint i64* %lnd8j to i64
  %lnd8l = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %lnd8m = bitcast i64* %lnd8l to i64*
  %lnd8n = load i64, i64* %lnd8m, !tbaa !4
  %lnd8o = icmp ugt i64 %lnd8k, %lnd8n
  br i1 %lnd8o, label %ccYI, label %ccYH
ccYH:
  %lnd8r = load i64, i64* %R1_Var
  %lnd8s = add i64 %lnd8r, 15
  %lnd8t = inttoptr i64 %lnd8s to i64*
  %lnd8u = load i64, i64* %lnd8t, !tbaa !3
  store i64 %lnd8u, i64* %lscO9
  %lnd8w = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @scOo_info$def to i64
  %lnd8v = load i64*, i64** %Hp_Var
  %lnd8x = getelementptr inbounds i64, i64* %lnd8v, i32 -2
  store i64 %lnd8w, i64* %lnd8x, !tbaa !2
  %lnd8z = load i64, i64* %lscO9
  %lnd8y = load i64*, i64** %Hp_Var
  %lnd8A = getelementptr inbounds i64, i64* %lnd8y, i32 0
  store i64 %lnd8z, i64* %lnd8A, !tbaa !2
  %lnd8B = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccYA_info$def to i64
  %lnd8C = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %lnd8B, i64* %lnd8C, !tbaa !1
  %lnd8D = ptrtoint i8* @ghczmprim_GHCziTypes_True_closure to i64
  %lnd8E = add i64 %lnd8D, 2
  store i64 %lnd8E, i64* %R4_Var
  %lnd8F = load i64*, i64** %Hp_Var
  %lnd8G = getelementptr inbounds i64, i64* %lnd8F, i32 -2
  %lnd8H = ptrtoint i64* %lnd8G to i64
  store i64 %lnd8H, i64* %R3_Var
  %lnd8I = ptrtoint i8* @base_GHCziIOziHandleziFD_stdout_closure to i64
  store i64 %lnd8I, i64* %R2_Var
  %lnd8J = bitcast i8* @base_GHCziIOziHandleziText_hPutStr2_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd8K = load i64*, i64** %Hp_Var
  %lnd8L = load i64, i64* %R1_Var
  %lnd8M = load i64, i64* %R2_Var
  %lnd8N = load i64, i64* %R3_Var
  %lnd8O = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd8J( i64* %Base_Arg, i64* %Sp_Arg, i64* %lnd8K, i64 %lnd8L, i64 %lnd8M, i64 %lnd8N, i64 %lnd8O, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccYI:
  %lnd8P = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %lnd8P, !tbaa !4
  %lnd8Q = load i64, i64* %R1_Var
  store i64 %lnd8Q, i64* %R1_Var
  %lnd8R = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd8S = load i64*, i64** %Hp_Var
  %lnd8T = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd8R( i64* %Base_Arg, i64* %Sp_Arg, i64* %lnd8S, i64 %lnd8T, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccYA_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccYA_info$def to i8*)
define internal ghccc void @ccYA_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccYA_info$def to i64)),i64 48), i64 1, i64 55834574880}>
{
ccYA:
  %lccXg = alloca i64, i32 1
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnd8U = load i64*, i64** %Sp_Var
  %lnd8V = getelementptr inbounds i64, i64* %lnd8U, i32 1
  %lnd8W = bitcast i64* %lnd8V to i64*
  %lnd8X = load i64, i64* %lnd8W, !tbaa !1
  store i64 %lnd8X, i64* %lccXg
  %lnd8Z = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccYC_info$def to i64
  %lnd8Y = load i64*, i64** %Sp_Var
  %lnd90 = getelementptr inbounds i64, i64* %lnd8Y, i32 1
  store i64 %lnd8Z, i64* %lnd90, !tbaa !1
  %lnd91 = load i64, i64* %lccXg
  store i64 %lnd91, i64* %R1_Var
  %lnd92 = load i64*, i64** %Sp_Var
  %lnd93 = getelementptr inbounds i64, i64* %lnd92, i32 1
  %lnd94 = ptrtoint i64* %lnd93 to i64
  %lnd95 = inttoptr i64 %lnd94 to i64*
  store i64* %lnd95, i64** %Sp_Var
  %lnd96 = load i64, i64* %R1_Var
  %lnd97 = and i64 %lnd96, 7
  %lnd98 = icmp ne i64 %lnd97, 0
  br i1 %lnd98, label %ucZn, label %ccYK
ccYK:
  %lnd9a = load i64, i64* %R1_Var
  %lnd9b = inttoptr i64 %lnd9a to i64*
  %lnd9c = load i64, i64* %lnd9b, !tbaa !3
  %lnd9d = inttoptr i64 %lnd9c to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd9e = load i64*, i64** %Sp_Var
  %lnd9f = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd9d( i64* %Base_Arg, i64* %lnd9e, i64* %Hp_Arg, i64 %lnd9f, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ucZn:
  %lnd9g = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccYC_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd9h = load i64*, i64** %Sp_Var
  %lnd9i = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd9g( i64* %Base_Arg, i64* %lnd9h, i64* %Hp_Arg, i64 %lnd9i, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccYC_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccYC_info$def to i8*)
define internal ghccc void @ccYC_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccYC_info$def to i64)),i64 48), i64 0, i64 55834574880}>
{
ccYC:
  %lscOu = alloca i64, i32 1
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
  %lscOA = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnd9j = add i64 %R1_Arg, 7
  %lnd9k = inttoptr i64 %lnd9j to i64*
  %lnd9l = load i64, i64* %lnd9k, !tbaa !3
  store i64 %lnd9l, i64* %lscOu
  %lnd9m = add i64 %R1_Arg, 31
  %lnd9n = inttoptr i64 %lnd9m to i64*
  %lnd9o = load i64, i64* %lnd9n, !tbaa !3
  %lnd9p = bitcast i8* @close to i64 (i64)*
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
  %lnd9q = call ccc i64 (i64) %lnd9p( i64 %lnd9o ) nounwind
  store i64 %lnd9q, i64* %lscOA
  %lnd9r = load i64, i64* %lscOA
  %lnd9s = trunc i64 %lnd9r to i32
  %lnd9t = sext i32 %lnd9s to i64
  switch i64 %lnd9t, label %ccYW [i64 -1, label %ccZ5]
ccYW:
  %lnd9u = load i64, i64* %lscOu
  %lnd9v = add i64 %lnd9u, 8
  %lnd9w = inttoptr i64 %lnd9v to i64*
  %lnd9x = load i64, i64* %lnd9w, !tbaa !5
  store i64 %lnd9x, i64* %R2_Var
  %lnd9y = load i64*, i64** %Sp_Var
  %lnd9z = getelementptr inbounds i64, i64* %lnd9y, i32 1
  %lnd9A = ptrtoint i64* %lnd9z to i64
  %lnd9B = inttoptr i64 %lnd9A to i64*
  store i64* %lnd9B, i64** %Sp_Var
  %lnd9C = bitcast i8* @Disk_printzustats1_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd9D = load i64*, i64** %Sp_Var
  %lnd9E = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd9C( i64* %Base_Arg, i64* %lnd9D, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnd9E, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
ccZ5:
  %lnd9G = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccZ4_info$def to i64
  %lnd9F = load i64*, i64** %Sp_Var
  %lnd9H = getelementptr inbounds i64, i64* %lnd9F, i32 -1
  store i64 %lnd9G, i64* %lnd9H, !tbaa !1
  %lnd9I = ptrtoint i8* @unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_closeFd2_closure to i64
  store i64 %lnd9I, i64* %R2_Var
  %lnd9K = load i64, i64* %lscOu
  %lnd9J = load i64*, i64** %Sp_Var
  %lnd9L = getelementptr inbounds i64, i64* %lnd9J, i32 0
  store i64 %lnd9K, i64* %lnd9L, !tbaa !1
  %lnd9M = load i64*, i64** %Sp_Var
  %lnd9N = getelementptr inbounds i64, i64* %lnd9M, i32 -1
  %lnd9O = ptrtoint i64* %lnd9N to i64
  %lnd9P = inttoptr i64 %lnd9O to i64*
  store i64* %lnd9P, i64** %Sp_Var
  %lnd9Q = bitcast i8* @base_ForeignziCziError_throwErrno1_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnd9R = load i64*, i64** %Sp_Var
  %lnd9S = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnd9Q( i64* %Base_Arg, i64* %lnd9R, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnd9S, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@ccZ4_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccZ4_info$def to i8*)
define internal ghccc void @ccZ4_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ccZ4_info$def to i64)),i64 48), i64 1, i64 4294967328}>
{
ccZ4:
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %lnd9T = load i64*, i64** %Sp_Var
  %lnd9U = getelementptr inbounds i64, i64* %lnd9T, i32 1
  %lnd9V = bitcast i64* %lnd9U to i64*
  %lnd9W = load i64, i64* %lnd9V, !tbaa !1
  %lnd9X = add i64 %lnd9W, 8
  %lnd9Y = inttoptr i64 %lnd9X to i64*
  %lnd9Z = load i64, i64* %lnd9Y, !tbaa !5
  store i64 %lnd9Z, i64* %R2_Var
  %lnda0 = load i64*, i64** %Sp_Var
  %lnda1 = getelementptr inbounds i64, i64* %lnda0, i32 2
  %lnda2 = ptrtoint i64* %lnda1 to i64
  %lnda3 = inttoptr i64 %lnda2 to i64*
  store i64* %lnda3, i64** %Sp_Var
  %lnda4 = bitcast i8* @Disk_printzustats1_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lnda5 = load i64*, i64** %Sp_Var
  %lnda6 = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lnda4( i64* %Base_Arg, i64* %lnda5, i64* %Hp_Arg, i64 %R1_Arg, i64 %lnda6, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Main_main_closure_struct = type <{i64, i64}>
@Main_main_closure$def = internal global %Main_main_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Main_main_info$def to i64), i64 0}>
@Main_main_closure = alias i8* bitcast (%Main_main_closure_struct* @Main_main_closure$def to i8*)
@Main_main_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Main_main_info$def to i8*)
define ghccc void @Main_main_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Main_main_info$def to i64)),i64 104), i64 4294967299, i64 0, i64 4294967311}>
{
cdac:
  %lndaf = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Main_main1_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndaf( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Main_main2_closure_struct = type <{i64, i64}>
@Main_main2_closure$def = internal global %Main_main2_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Main_main2_info$def to i64), i64 0}>
@Main_main2_closure = alias i8* bitcast (%Main_main2_closure_struct* @Main_main2_closure$def to i8*)
@Main_main2_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Main_main2_info$def to i8*)
define ghccc void @Main_main2_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Main_main2_info$def to i64)),i64 104), i64 4294967299, i64 0, i64 141733920783}>
{
cdal:
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %lndao = ptrtoint %Main_main1_closure_struct* @Main_main1_closure$def to i64
  %lndap = add i64 %lndao, 1
  store i64 %lndap, i64* %R2_Var
  %lndaq = bitcast i8* @base_GHCziTopHandler_runMainIO1_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %lndar = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndaq( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %lndar, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%ZCMain_main_closure_struct = type <{i64, i64}>
@ZCMain_main_closure$def = internal global %ZCMain_main_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ZCMain_main_info$def to i64), i64 0}>
@ZCMain_main_closure = alias i8* bitcast (%ZCMain_main_closure_struct* @ZCMain_main_closure$def to i8*)
@ZCMain_main_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ZCMain_main_info$def to i8*)
define ghccc void @ZCMain_main_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @ScPT_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @ZCMain_main_info$def to i64)),i64 152), i64 4294967299, i64 0, i64 4294967311}>
{
cdax:
  %lndaA = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Main_main2_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %lndaA( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%ScPT_srt_struct = type <{i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64, i64}>
@ScPT_srt$def = internal constant %ScPT_srt_struct<{i64 ptrtoint (i8* @base_GHCziIOziHandleziFD_stdout_closure to i64), i64 ptrtoint (i8* @base_GHCziIOziHandleziText_hPutStr2_closure to i64), i64 ptrtoint (%rcN8_closure_struct* @rcN8_closure$def to i64), i64 ptrtoint (i8* @base_GHCziShow_zdwzdcshowsPrec3_closure to i64), i64 ptrtoint (%rcNa_closure_struct* @rcNa_closure$def to i64), i64 ptrtoint (i8* @base_GHCziErr_error_closure to i64), i64 ptrtoint (i8* @Disk_printzustats1_closure to i64), i64 ptrtoint (i8* @unixzm2zi7zi2zi0_SystemziPosixziIO_createFile3_closure to i64), i64 ptrtoint (i8* @unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_closeFd2_closure to i64), i64 ptrtoint (i8* @base_ForeignziCziError_throwErrno1_closure to i64), i64 ptrtoint (i8* @AsyncFS_zdwzuAFSzuzumkfs_closure to i64), i64 ptrtoint (i8* @Interpreter_run2_closure to i64), i64 ptrtoint (i8* @base_SystemziEnvironment_getArgs1_closure to i64), i64 ptrtoint (%Main_main1_closure_struct* @Main_main1_closure$def to i64), i64 ptrtoint (%rcN9_closure_struct* @rcN9_closure$def to i64), i64 ptrtoint (%rcNj_closure_struct* @rcNj_closure$def to i64), i64 ptrtoint (%rcNk_closure_struct* @rcNk_closure$def to i64), i64 ptrtoint (%rcNo_closure_struct* @rcNo_closure$def to i64), i64 ptrtoint (i8* @base_GHCziTopHandler_runMainIO1_closure to i64), i64 ptrtoint (%Main_main2_closure_struct* @Main_main2_closure$def to i64)}>
@ScPT_srt = internal alias i8* bitcast (%ScPT_srt_struct* @ScPT_srt$def to i8*)
@close = external global i8
@integerzmgmp_GHCziIntegerziType_Szh_static_info = external global i8
@ghczmprim_GHCziTypes_TrNameS_static_info = external global i8
@ghczmprim_GHCziTypes_Module_static_info = external global i8
@newCAF = external global i8
@stg_bh_upd_frame_info = external global i8
@ghczmprim_GHCziCString_unpackCStringzh_info = external global i8
@ghczmprim_GHCziTypes_True_closure = external global i8
@base_GHCziIOziHandleziFD_stdout_closure = external global i8
@base_GHCziIOziHandleziText_hPutStr2_info = external global i8
@ghczmprim_GHCziTypes_Izh_static_info = external global i8
@base_GHCziStackziTypes_SrcLoc_static_info = external global i8
@base_GHCziStackziTypes_PushCallStack_static_info = external global i8
@base_GHCziStackziTypes_EmptyCallStack_closure = external global i8
@stg_upd_frame_info = external global i8
@Disk_S_con_info = external global i8
@base_GHCziBase_Nothing_closure = external global i8
@stg_gc_unpt_r1 = external global i8
@ghczmprim_GHCziTypes_ZMZN_closure = external global i8
@base_GHCziShow_zdwzdcshowsPrec3_info = external global i8
@ghczmprim_GHCziTypes_ZC_con_info = external global i8
@base_GHCziBase_zpzp_info = external global i8
@stg_gc_pp = external global i8
@ghczmprim_GHCziCString_unpackAppendCStringzh_info = external global i8
@base_SystemziEnvironment_getArgs1_info = external global i8
@unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_defaultFileFlags_closure = external global i8
@Disk_initzudisk4_closure = external global i8
@unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_ReadWrite_closure = external global i8
@unixzm2zi7zi2zi0_SystemziPosixziIO_createFile3_info = external global i8
@Disk_clearzustats2_closure = external global i8
@stg_newMutVarzh = external global i8
@Disk_initzudisk2_closure = external global i8
@AsyncFS_zdwzuAFSzuzumkfs_info = external global i8
@Prog_Bind_con_info = external global i8
@Interpreter_run2_info = external global i8
@base_GHCziErr_error_info = external global i8
@Disk_printzustats1_info = external global i8
@unixzm2zi7zi2zi0_SystemziPosixziIOziCommon_closeFd2_closure = external global i8
@base_ForeignziCziError_throwErrno1_info = external global i8
@base_GHCziTopHandler_runMainIO1_info = external global i8
@base_GHCziIOziHandleziText_hPutStr2_closure = external global i8
@base_GHCziShow_zdwzdcshowsPrec3_closure = external global i8
@base_GHCziErr_error_closure = external global i8
@Disk_printzustats1_closure = external global i8
@unixzm2zi7zi2zi0_SystemziPosixziIO_createFile3_closure = external global i8
@base_ForeignziCziError_throwErrno1_closure = external global i8
@AsyncFS_zdwzuAFSzuzumkfs_closure = external global i8
@Interpreter_run2_closure = external global i8
@base_SystemziEnvironment_getArgs1_closure = external global i8
@base_GHCziTopHandler_runMainIO1_closure = external global i8
@llvm.used = appending constant [38 x i8*] [i8* bitcast (%ScPT_srt_struct* @ScPT_srt$def to i8*), i8* bitcast (%ZCMain_main_closure_struct* @ZCMain_main_closure$def to i8*), i8* bitcast (%Main_main2_closure_struct* @Main_main2_closure$def to i8*), i8* bitcast (%Main_main_closure_struct* @Main_main_closure$def to i8*), i8* bitcast (%ccY8_str_struct* @ccY8_str$def to i8*), i8* bitcast (%Main_main1_closure_struct* @Main_main1_closure$def to i8*), i8* bitcast (%ccVp_str_struct* @ccVp_str$def to i8*), i8* bitcast (%rcNo_closure_struct* @rcNo_closure$def to i8*), i8* bitcast (%rcNn_closure_struct* @rcNn_closure$def to i8*), i8* bitcast (%rcNm_closure_struct* @rcNm_closure$def to i8*), i8* bitcast (%rcNl_closure_struct* @rcNl_closure$def to i8*), i8* bitcast (%ccUv_str_struct* @ccUv_str$def to i8*), i8* bitcast (%rcNk_closure_struct* @rcNk_closure$def to i8*), i8* bitcast (%rcNj_closure_struct* @rcNj_closure$def to i8*), i8* bitcast (%rcNi_closure_struct* @rcNi_closure$def to i8*), i8* bitcast (%rcNh_closure_struct* @rcNh_closure$def to i8*), i8* bitcast (%rcNg_closure_struct* @rcNg_closure$def to i8*), i8* bitcast (%rcNf_closure_struct* @rcNf_closure$def to i8*), i8* bitcast (%ccTz_str_struct* @ccTz_str$def to i8*), i8* bitcast (%rcNe_closure_struct* @rcNe_closure$def to i8*), i8* bitcast (%ccSI_str_struct* @ccSI_str$def to i8*), i8* bitcast (%rcNd_closure_struct* @rcNd_closure$def to i8*), i8* bitcast (%ccRR_str_struct* @ccRR_str$def to i8*), i8* bitcast (%rcNc_closure_struct* @rcNc_closure$def to i8*), i8* bitcast (%ccR0_str_struct* @ccR0_str$def to i8*), i8* bitcast (%rcNb_closure_struct* @rcNb_closure$def to i8*), i8* bitcast (%ccQ9_str_struct* @ccQ9_str$def to i8*), i8* bitcast (%rcNa_closure_struct* @rcNa_closure$def to i8*), i8* bitcast (%rcN9_closure_struct* @rcN9_closure$def to i8*), i8* bitcast (%ccP1_str_struct* @ccP1_str$def to i8*), i8* bitcast (%rcN8_closure_struct* @rcN8_closure$def to i8*), i8* bitcast (%Main_zdtrModule_closure_struct* @Main_zdtrModule_closure$def to i8*), i8* bitcast (%Main_zdtrModule1_closure_struct* @Main_zdtrModule1_closure$def to i8*), i8* bitcast (%ccOS_str_struct* @ccOS_str$def to i8*), i8* bitcast (%Main_zdtrModule2_closure_struct* @Main_zdtrModule2_closure$def to i8*), i8* bitcast (%ccOQ_str_struct* @ccOQ_str$def to i8*), i8* bitcast (%Main_cachesizze_closure_struct* @Main_cachesizze_closure$def to i8*), i8* bitcast (%__stginit_Main_struct* @__stginit_Main$def to i8*)], section "llvm.metadata"
