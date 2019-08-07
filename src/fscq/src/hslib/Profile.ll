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
%__stginit_Profile_struct = type <{}>
@__stginit_Profile$def = internal global %__stginit_Profile_struct<{}>
@__stginit_Profile = alias i8* bitcast (%__stginit_Profile_struct* @__stginit_Profile$def to i8*)
%Profile_doTraceFile1_closure_struct = type <{i64, i64, i64, i64}>
%c17C_str_struct = type <{[14 x i8]}>
@Profile_doTraceFile1_closure$def = internal global %Profile_doTraceFile1_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Profile_doTraceFile1_info$def to i64), i64 0, i64 0, i64 0}>
@Profile_doTraceFile1_closure = alias i8* bitcast (%Profile_doTraceFile1_closure_struct* @Profile_doTraceFile1_closure$def to i8*)
@c17C_str$def = internal constant %c17C_str_struct<{[14 x i8] [i8 99, i8 111, i8 100, i8 101, i8 103, i8 101, i8 110, i8 47, i8 70, i8 83, i8 46, i8 104, i8 115, i8 0]}>
@c17C_str = internal alias i8* bitcast (%c17C_str_struct* @c17C_str$def to i8*)
@Profile_doTraceFile1_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Profile_doTraceFile1_info$def to i8*)
define ghccc void @Profile_doTraceFile1_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 22}>
{
c17D:
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
  %lc17z = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %ln17G = load i64*, i64** %Sp_Var
  %ln17H = getelementptr inbounds i64, i64* %ln17G, i32 -2
  %ln17I = ptrtoint i64* %ln17H to i64
  %ln17J = icmp ult i64 %ln17I, %SpLim_Arg
  br i1 %ln17J, label %c17E, label %c17F
c17F:
  %ln17K = ptrtoint i64* %Base_Arg to i64
  %ln17L = inttoptr i64 %ln17K to i8*
  %ln17M = load i64, i64* %R1_Var
  %ln17N = inttoptr i64 %ln17M to i8*
  %ln17O = bitcast i8* @newCAF to i8* (i8*, i8*)*
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
  %ln17P = call ccc i8* (i8*, i8*) %ln17O( i8* %ln17L, i8* %ln17N ) nounwind
  %ln17Q = ptrtoint i8* %ln17P to i64
  store i64 %ln17Q, i64* %lc17z
  %ln17R = load i64, i64* %lc17z
  %ln17S = icmp eq i64 %ln17R, 0
  br i1 %ln17S, label %c17B, label %c17A
c17A:
  %ln17U = ptrtoint i8* @stg_bh_upd_frame_info to i64
  %ln17T = load i64*, i64** %Sp_Var
  %ln17V = getelementptr inbounds i64, i64* %ln17T, i32 -2
  store i64 %ln17U, i64* %ln17V, !tbaa !1
  %ln17X = load i64, i64* %lc17z
  %ln17W = load i64*, i64** %Sp_Var
  %ln17Y = getelementptr inbounds i64, i64* %ln17W, i32 -1
  store i64 %ln17X, i64* %ln17Y, !tbaa !1
  %ln17Z = ptrtoint %c17C_str_struct* @c17C_str$def to i64
  store i64 %ln17Z, i64* %R2_Var
  %ln180 = load i64*, i64** %Sp_Var
  %ln181 = getelementptr inbounds i64, i64* %ln180, i32 -2
  %ln182 = ptrtoint i64* %ln181 to i64
  %ln183 = inttoptr i64 %ln182 to i64*
  store i64* %ln183, i64** %Sp_Var
  %ln184 = bitcast i8* @ghczmprim_GHCziCString_unpackCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln185 = load i64*, i64** %Sp_Var
  %ln186 = load i64, i64* %R1_Var
  %ln187 = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln184( i64* %Base_Arg, i64* %ln185, i64* %Hp_Arg, i64 %ln186, i64 %ln187, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
c17B:
  %ln189 = load i64, i64* %R1_Var
  %ln18a = inttoptr i64 %ln189 to i64*
  %ln18b = load i64, i64* %ln18a, !tbaa !3
  %ln18c = inttoptr i64 %ln18b to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln18d = load i64*, i64** %Sp_Var
  %ln18e = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln18c( i64* %Base_Arg, i64* %ln18d, i64* %Hp_Arg, i64 %ln18e, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
c17E:
  %ln18f = load i64, i64* %R1_Var
  store i64 %ln18f, i64* %R1_Var
  %ln18g = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %ln18h = bitcast i64* %ln18g to i64*
  %ln18i = load i64, i64* %ln18h, !tbaa !4
  %ln18j = inttoptr i64 %ln18i to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln18k = load i64*, i64** %Sp_Var
  %ln18l = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln18j( i64* %Base_Arg, i64* %ln18k, i64* %Hp_Arg, i64 %ln18l, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Profile_doTraceFile2_closure_struct = type <{i64, i64, i64, i64}>
%c18t_str_struct = type <{[19 x i8]}>
@Profile_doTraceFile2_closure$def = internal global %Profile_doTraceFile2_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Profile_doTraceFile2_info$def to i64), i64 0, i64 0, i64 0}>
@Profile_doTraceFile2_closure = alias i8* bitcast (%Profile_doTraceFile2_closure_struct* @Profile_doTraceFile2_closure$def to i8*)
@c18t_str$def = internal constant %c18t_str_struct<{[19 x i8] [i8 99, i8 111, i8 100, i8 101, i8 103, i8 101, i8 110, i8 47, i8 68, i8 105, i8 114, i8 84, i8 114, i8 101, i8 101, i8 46, i8 104, i8 115, i8 0]}>
@c18t_str = internal alias i8* bitcast (%c18t_str_struct* @c18t_str$def to i8*)
@Profile_doTraceFile2_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Profile_doTraceFile2_info$def to i8*)
define ghccc void @Profile_doTraceFile2_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 22}>
{
c18u:
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
  %lc18q = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %ln18x = load i64*, i64** %Sp_Var
  %ln18y = getelementptr inbounds i64, i64* %ln18x, i32 -2
  %ln18z = ptrtoint i64* %ln18y to i64
  %ln18A = icmp ult i64 %ln18z, %SpLim_Arg
  br i1 %ln18A, label %c18v, label %c18w
c18w:
  %ln18B = ptrtoint i64* %Base_Arg to i64
  %ln18C = inttoptr i64 %ln18B to i8*
  %ln18D = load i64, i64* %R1_Var
  %ln18E = inttoptr i64 %ln18D to i8*
  %ln18F = bitcast i8* @newCAF to i8* (i8*, i8*)*
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
  %ln18G = call ccc i8* (i8*, i8*) %ln18F( i8* %ln18C, i8* %ln18E ) nounwind
  %ln18H = ptrtoint i8* %ln18G to i64
  store i64 %ln18H, i64* %lc18q
  %ln18I = load i64, i64* %lc18q
  %ln18J = icmp eq i64 %ln18I, 0
  br i1 %ln18J, label %c18s, label %c18r
c18r:
  %ln18L = ptrtoint i8* @stg_bh_upd_frame_info to i64
  %ln18K = load i64*, i64** %Sp_Var
  %ln18M = getelementptr inbounds i64, i64* %ln18K, i32 -2
  store i64 %ln18L, i64* %ln18M, !tbaa !1
  %ln18O = load i64, i64* %lc18q
  %ln18N = load i64*, i64** %Sp_Var
  %ln18P = getelementptr inbounds i64, i64* %ln18N, i32 -1
  store i64 %ln18O, i64* %ln18P, !tbaa !1
  %ln18Q = ptrtoint %c18t_str_struct* @c18t_str$def to i64
  store i64 %ln18Q, i64* %R2_Var
  %ln18R = load i64*, i64** %Sp_Var
  %ln18S = getelementptr inbounds i64, i64* %ln18R, i32 -2
  %ln18T = ptrtoint i64* %ln18S to i64
  %ln18U = inttoptr i64 %ln18T to i64*
  store i64* %ln18U, i64** %Sp_Var
  %ln18V = bitcast i8* @ghczmprim_GHCziCString_unpackCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln18W = load i64*, i64** %Sp_Var
  %ln18X = load i64, i64* %R1_Var
  %ln18Y = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln18V( i64* %Base_Arg, i64* %ln18W, i64* %Hp_Arg, i64 %ln18X, i64 %ln18Y, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
c18s:
  %ln190 = load i64, i64* %R1_Var
  %ln191 = inttoptr i64 %ln190 to i64*
  %ln192 = load i64, i64* %ln191, !tbaa !3
  %ln193 = inttoptr i64 %ln192 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln194 = load i64*, i64** %Sp_Var
  %ln195 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln193( i64* %Base_Arg, i64* %ln194, i64* %Hp_Arg, i64 %ln195, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
c18v:
  %ln196 = load i64, i64* %R1_Var
  store i64 %ln196, i64* %R1_Var
  %ln197 = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %ln198 = bitcast i64* %ln197 to i64*
  %ln199 = load i64, i64* %ln198, !tbaa !4
  %ln19a = inttoptr i64 %ln199 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln19b = load i64*, i64** %Sp_Var
  %ln19c = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln19a( i64* %Base_Arg, i64* %ln19b, i64* %Hp_Arg, i64 %ln19c, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Profile_doTraceFile3_closure_struct = type <{i64, i64, i64, i64}>
%c19k_str_struct = type <{[19 x i8]}>
@Profile_doTraceFile3_closure$def = internal global %Profile_doTraceFile3_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Profile_doTraceFile3_info$def to i64), i64 0, i64 0, i64 0}>
@Profile_doTraceFile3_closure = alias i8* bitcast (%Profile_doTraceFile3_closure_struct* @Profile_doTraceFile3_closure$def to i8*)
@c19k_str$def = internal constant %c19k_str_struct<{[19 x i8] [i8 99, i8 111, i8 100, i8 101, i8 103, i8 101, i8 110, i8 47, i8 68, i8 105, i8 114, i8 78, i8 97, i8 109, i8 101, i8 46, i8 104, i8 115, i8 0]}>
@c19k_str = internal alias i8* bitcast (%c19k_str_struct* @c19k_str$def to i8*)
@Profile_doTraceFile3_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Profile_doTraceFile3_info$def to i8*)
define ghccc void @Profile_doTraceFile3_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 22}>
{
c19l:
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
  %lc19h = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %ln19o = load i64*, i64** %Sp_Var
  %ln19p = getelementptr inbounds i64, i64* %ln19o, i32 -2
  %ln19q = ptrtoint i64* %ln19p to i64
  %ln19r = icmp ult i64 %ln19q, %SpLim_Arg
  br i1 %ln19r, label %c19m, label %c19n
c19n:
  %ln19s = ptrtoint i64* %Base_Arg to i64
  %ln19t = inttoptr i64 %ln19s to i8*
  %ln19u = load i64, i64* %R1_Var
  %ln19v = inttoptr i64 %ln19u to i8*
  %ln19w = bitcast i8* @newCAF to i8* (i8*, i8*)*
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
  %ln19x = call ccc i8* (i8*, i8*) %ln19w( i8* %ln19t, i8* %ln19v ) nounwind
  %ln19y = ptrtoint i8* %ln19x to i64
  store i64 %ln19y, i64* %lc19h
  %ln19z = load i64, i64* %lc19h
  %ln19A = icmp eq i64 %ln19z, 0
  br i1 %ln19A, label %c19j, label %c19i
c19i:
  %ln19C = ptrtoint i8* @stg_bh_upd_frame_info to i64
  %ln19B = load i64*, i64** %Sp_Var
  %ln19D = getelementptr inbounds i64, i64* %ln19B, i32 -2
  store i64 %ln19C, i64* %ln19D, !tbaa !1
  %ln19F = load i64, i64* %lc19h
  %ln19E = load i64*, i64** %Sp_Var
  %ln19G = getelementptr inbounds i64, i64* %ln19E, i32 -1
  store i64 %ln19F, i64* %ln19G, !tbaa !1
  %ln19H = ptrtoint %c19k_str_struct* @c19k_str$def to i64
  store i64 %ln19H, i64* %R2_Var
  %ln19I = load i64*, i64** %Sp_Var
  %ln19J = getelementptr inbounds i64, i64* %ln19I, i32 -2
  %ln19K = ptrtoint i64* %ln19J to i64
  %ln19L = inttoptr i64 %ln19K to i64*
  store i64* %ln19L, i64** %Sp_Var
  %ln19M = bitcast i8* @ghczmprim_GHCziCString_unpackCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln19N = load i64*, i64** %Sp_Var
  %ln19O = load i64, i64* %R1_Var
  %ln19P = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln19M( i64* %Base_Arg, i64* %ln19N, i64* %Hp_Arg, i64 %ln19O, i64 %ln19P, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
c19j:
  %ln19R = load i64, i64* %R1_Var
  %ln19S = inttoptr i64 %ln19R to i64*
  %ln19T = load i64, i64* %ln19S, !tbaa !3
  %ln19U = inttoptr i64 %ln19T to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln19V = load i64*, i64** %Sp_Var
  %ln19W = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln19U( i64* %Base_Arg, i64* %ln19V, i64* %Hp_Arg, i64 %ln19W, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
c19m:
  %ln19X = load i64, i64* %R1_Var
  store i64 %ln19X, i64* %R1_Var
  %ln19Y = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %ln19Z = bitcast i64* %ln19Y to i64*
  %ln1a0 = load i64, i64* %ln19Z, !tbaa !4
  %ln1a1 = inttoptr i64 %ln1a0 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1a2 = load i64*, i64** %Sp_Var
  %ln1a3 = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1a1( i64* %Base_Arg, i64* %ln1a2, i64* %Hp_Arg, i64 %ln1a3, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Profile_doTraceFile4_closure_struct = type <{i64, i64, i64, i64}>
%c1ab_str_struct = type <{[15 x i8]}>
@Profile_doTraceFile4_closure$def = internal global %Profile_doTraceFile4_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Profile_doTraceFile4_info$def to i64), i64 0, i64 0, i64 0}>
@Profile_doTraceFile4_closure = alias i8* bitcast (%Profile_doTraceFile4_closure_struct* @Profile_doTraceFile4_closure$def to i8*)
@c1ab_str$def = internal constant %c1ab_str_struct<{[15 x i8] [i8 99, i8 111, i8 100, i8 101, i8 103, i8 101, i8 110, i8 47, i8 68, i8 105, i8 114, i8 46, i8 104, i8 115, i8 0]}>
@c1ab_str = internal alias i8* bitcast (%c1ab_str_struct* @c1ab_str$def to i8*)
@Profile_doTraceFile4_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Profile_doTraceFile4_info$def to i8*)
define ghccc void @Profile_doTraceFile4_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 22}>
{
c1ac:
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
  %lc1a8 = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %ln1af = load i64*, i64** %Sp_Var
  %ln1ag = getelementptr inbounds i64, i64* %ln1af, i32 -2
  %ln1ah = ptrtoint i64* %ln1ag to i64
  %ln1ai = icmp ult i64 %ln1ah, %SpLim_Arg
  br i1 %ln1ai, label %c1ad, label %c1ae
c1ae:
  %ln1aj = ptrtoint i64* %Base_Arg to i64
  %ln1ak = inttoptr i64 %ln1aj to i8*
  %ln1al = load i64, i64* %R1_Var
  %ln1am = inttoptr i64 %ln1al to i8*
  %ln1an = bitcast i8* @newCAF to i8* (i8*, i8*)*
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
  %ln1ao = call ccc i8* (i8*, i8*) %ln1an( i8* %ln1ak, i8* %ln1am ) nounwind
  %ln1ap = ptrtoint i8* %ln1ao to i64
  store i64 %ln1ap, i64* %lc1a8
  %ln1aq = load i64, i64* %lc1a8
  %ln1ar = icmp eq i64 %ln1aq, 0
  br i1 %ln1ar, label %c1aa, label %c1a9
c1a9:
  %ln1at = ptrtoint i8* @stg_bh_upd_frame_info to i64
  %ln1as = load i64*, i64** %Sp_Var
  %ln1au = getelementptr inbounds i64, i64* %ln1as, i32 -2
  store i64 %ln1at, i64* %ln1au, !tbaa !1
  %ln1aw = load i64, i64* %lc1a8
  %ln1av = load i64*, i64** %Sp_Var
  %ln1ax = getelementptr inbounds i64, i64* %ln1av, i32 -1
  store i64 %ln1aw, i64* %ln1ax, !tbaa !1
  %ln1ay = ptrtoint %c1ab_str_struct* @c1ab_str$def to i64
  store i64 %ln1ay, i64* %R2_Var
  %ln1az = load i64*, i64** %Sp_Var
  %ln1aA = getelementptr inbounds i64, i64* %ln1az, i32 -2
  %ln1aB = ptrtoint i64* %ln1aA to i64
  %ln1aC = inttoptr i64 %ln1aB to i64*
  store i64* %ln1aC, i64** %Sp_Var
  %ln1aD = bitcast i8* @ghczmprim_GHCziCString_unpackCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1aE = load i64*, i64** %Sp_Var
  %ln1aF = load i64, i64* %R1_Var
  %ln1aG = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1aD( i64* %Base_Arg, i64* %ln1aE, i64* %Hp_Arg, i64 %ln1aF, i64 %ln1aG, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
c1aa:
  %ln1aI = load i64, i64* %R1_Var
  %ln1aJ = inttoptr i64 %ln1aI to i64*
  %ln1aK = load i64, i64* %ln1aJ, !tbaa !3
  %ln1aL = inttoptr i64 %ln1aK to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1aM = load i64*, i64** %Sp_Var
  %ln1aN = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1aL( i64* %Base_Arg, i64* %ln1aM, i64* %Hp_Arg, i64 %ln1aN, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
c1ad:
  %ln1aO = load i64, i64* %R1_Var
  store i64 %ln1aO, i64* %R1_Var
  %ln1aP = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %ln1aQ = bitcast i64* %ln1aP to i64*
  %ln1aR = load i64, i64* %ln1aQ, !tbaa !4
  %ln1aS = inttoptr i64 %ln1aR to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1aT = load i64*, i64** %Sp_Var
  %ln1aU = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1aS( i64* %Base_Arg, i64* %ln1aT, i64* %Hp_Arg, i64 %ln1aU, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Profile_doTraceFile_closure_struct = type <{i64, i64}>
@Profile_doTraceFile_closure$def = internal global %Profile_doTraceFile_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Profile_doTraceFile_info$def to i64), i64 0}>
@Profile_doTraceFile_closure = alias i8* bitcast (%Profile_doTraceFile_closure_struct* @Profile_doTraceFile_closure$def to i8*)
@Profile_doTraceFile_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Profile_doTraceFile_info$def to i8*)
define ghccc void @Profile_doTraceFile_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @S1bV_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Profile_doTraceFile_info$def to i64)),i64 0), i64 4294967301, i64 0, i64 133143986191}>
{
c1bi:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %ls17b = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %ln1bW = load i64*, i64** %Sp_Var
  %ln1bX = getelementptr inbounds i64, i64* %ln1bW, i32 -2
  %ln1bY = ptrtoint i64* %ln1bX to i64
  %ln1bZ = icmp ult i64 %ln1bY, %SpLim_Arg
  br i1 %ln1bZ, label %c1bj, label %c1bk
c1bk:
  %ln1c1 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @c1bc_info$def to i64
  %ln1c0 = load i64*, i64** %Sp_Var
  %ln1c2 = getelementptr inbounds i64, i64* %ln1c0, i32 -2
  store i64 %ln1c1, i64* %ln1c2, !tbaa !1
  %ln1c3 = ptrtoint %Profile_doTraceFile4_closure_struct* @Profile_doTraceFile4_closure$def to i64
  store i64 %ln1c3, i64* %R3_Var
  %ln1c4 = load i64, i64* %R2_Var
  store i64 %ln1c4, i64* %ls17b
  %ln1c5 = load i64, i64* %R2_Var
  store i64 %ln1c5, i64* %R2_Var
  %ln1c7 = load i64, i64* %ls17b
  %ln1c6 = load i64*, i64** %Sp_Var
  %ln1c8 = getelementptr inbounds i64, i64* %ln1c6, i32 -1
  store i64 %ln1c7, i64* %ln1c8, !tbaa !1
  %ln1c9 = load i64*, i64** %Sp_Var
  %ln1ca = getelementptr inbounds i64, i64* %ln1c9, i32 -2
  %ln1cb = ptrtoint i64* %ln1ca to i64
  %ln1cc = inttoptr i64 %ln1cb to i64*
  store i64* %ln1cc, i64** %Sp_Var
  %ln1cd = bitcast i8* @base_GHCziBase_eqString_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1ce = load i64*, i64** %Sp_Var
  %ln1cf = load i64, i64* %R1_Var
  %ln1cg = load i64, i64* %R2_Var
  %ln1ch = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1cd( i64* %Base_Arg, i64* %ln1ce, i64* %Hp_Arg, i64 %ln1cf, i64 %ln1cg, i64 %ln1ch, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
c1bj:
  %ln1ci = load i64, i64* %R2_Var
  store i64 %ln1ci, i64* %R2_Var
  %ln1cj = ptrtoint %Profile_doTraceFile_closure_struct* @Profile_doTraceFile_closure$def to i64
  store i64 %ln1cj, i64* %R1_Var
  %ln1ck = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %ln1cl = bitcast i64* %ln1ck to i64*
  %ln1cm = load i64, i64* %ln1cl, !tbaa !4
  %ln1cn = inttoptr i64 %ln1cm to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1co = load i64*, i64** %Sp_Var
  %ln1cp = load i64, i64* %R1_Var
  %ln1cq = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1cn( i64* %Base_Arg, i64* %ln1co, i64* %Hp_Arg, i64 %ln1cp, i64 %ln1cq, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@c1bc_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @c1bc_info$def to i8*)
define internal ghccc void @c1bc_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @S1bV_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @c1bc_info$def to i64)),i64 0), i64 1, i64 30064771104}>
{
c1bc:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %ln1cr = and i64 %R1_Arg, 7
  switch i64 %ln1cr, label %c1bf [i64 1, label %c1bf
i64 2, label %u1bP]
c1bf:
  %ln1ct = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @c1bn_info$def to i64
  %ln1cs = load i64*, i64** %Sp_Var
  %ln1cu = getelementptr inbounds i64, i64* %ln1cs, i32 0
  store i64 %ln1ct, i64* %ln1cu, !tbaa !1
  %ln1cv = ptrtoint %Profile_doTraceFile3_closure_struct* @Profile_doTraceFile3_closure$def to i64
  store i64 %ln1cv, i64* %R3_Var
  %ln1cw = load i64*, i64** %Sp_Var
  %ln1cx = getelementptr inbounds i64, i64* %ln1cw, i32 1
  %ln1cy = bitcast i64* %ln1cx to i64*
  %ln1cz = load i64, i64* %ln1cy, !tbaa !1
  store i64 %ln1cz, i64* %R2_Var
  %ln1cA = bitcast i8* @base_GHCziBase_eqString_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1cB = load i64*, i64** %Sp_Var
  %ln1cC = load i64, i64* %R2_Var
  %ln1cD = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1cA( i64* %Base_Arg, i64* %ln1cB, i64* %Hp_Arg, i64 %R1_Arg, i64 %ln1cC, i64 %ln1cD, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
u1bP:
  %ln1cE = load i64*, i64** %Sp_Var
  %ln1cF = getelementptr inbounds i64, i64* %ln1cE, i32 2
  %ln1cG = ptrtoint i64* %ln1cF to i64
  %ln1cH = inttoptr i64 %ln1cG to i64*
  store i64* %ln1cH, i64** %Sp_Var
  %ln1cI = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @c1bg_entry$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1cJ = load i64*, i64** %Sp_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1cI( i64* %Base_Arg, i64* %ln1cJ, i64* %Hp_Arg, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@c1bn_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @c1bn_info$def to i8*)
define internal ghccc void @c1bn_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @S1bV_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @c1bn_info$def to i64)),i64 0), i64 1, i64 12884901920}>
{
c1bn:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %ln1cK = and i64 %R1_Arg, 7
  switch i64 %ln1cK, label %c1bv [i64 1, label %c1bv
i64 2, label %u1bQ]
c1bv:
  %ln1cM = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @c1bs_info$def to i64
  %ln1cL = load i64*, i64** %Sp_Var
  %ln1cN = getelementptr inbounds i64, i64* %ln1cL, i32 0
  store i64 %ln1cM, i64* %ln1cN, !tbaa !1
  %ln1cO = ptrtoint %Profile_doTraceFile2_closure_struct* @Profile_doTraceFile2_closure$def to i64
  store i64 %ln1cO, i64* %R3_Var
  %ln1cP = load i64*, i64** %Sp_Var
  %ln1cQ = getelementptr inbounds i64, i64* %ln1cP, i32 1
  %ln1cR = bitcast i64* %ln1cQ to i64*
  %ln1cS = load i64, i64* %ln1cR, !tbaa !1
  store i64 %ln1cS, i64* %R2_Var
  %ln1cT = bitcast i8* @base_GHCziBase_eqString_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1cU = load i64*, i64** %Sp_Var
  %ln1cV = load i64, i64* %R2_Var
  %ln1cW = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1cT( i64* %Base_Arg, i64* %ln1cU, i64* %Hp_Arg, i64 %R1_Arg, i64 %ln1cV, i64 %ln1cW, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
u1bQ:
  %ln1cX = load i64*, i64** %Sp_Var
  %ln1cY = getelementptr inbounds i64, i64* %ln1cX, i32 2
  %ln1cZ = ptrtoint i64* %ln1cY to i64
  %ln1d0 = inttoptr i64 %ln1cZ to i64*
  store i64* %ln1d0, i64** %Sp_Var
  %ln1d1 = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @c1bg_entry$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1d2 = load i64*, i64** %Sp_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1d1( i64* %Base_Arg, i64* %ln1d2, i64* %Hp_Arg, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@c1bs_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @c1bs_info$def to i8*)
define internal ghccc void @c1bs_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @S1bV_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @c1bs_info$def to i64)),i64 0), i64 1, i64 4294967328}>
{
c1bs:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %ln1d3 = and i64 %R1_Arg, 7
  switch i64 %ln1d3, label %c1bB [i64 1, label %c1bB
i64 2, label %u1bR]
c1bB:
  %ln1d4 = ptrtoint %Profile_doTraceFile1_closure_struct* @Profile_doTraceFile1_closure$def to i64
  store i64 %ln1d4, i64* %R3_Var
  %ln1d5 = load i64*, i64** %Sp_Var
  %ln1d6 = getelementptr inbounds i64, i64* %ln1d5, i32 1
  %ln1d7 = bitcast i64* %ln1d6 to i64*
  %ln1d8 = load i64, i64* %ln1d7, !tbaa !1
  store i64 %ln1d8, i64* %R2_Var
  %ln1d9 = load i64*, i64** %Sp_Var
  %ln1da = getelementptr inbounds i64, i64* %ln1d9, i32 2
  %ln1db = ptrtoint i64* %ln1da to i64
  %ln1dc = inttoptr i64 %ln1db to i64*
  store i64* %ln1dc, i64** %Sp_Var
  %ln1dd = bitcast i8* @base_GHCziBase_eqString_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1de = load i64*, i64** %Sp_Var
  %ln1df = load i64, i64* %R2_Var
  %ln1dg = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1dd( i64* %Base_Arg, i64* %ln1de, i64* %Hp_Arg, i64 %R1_Arg, i64 %ln1df, i64 %ln1dg, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
u1bR:
  %ln1dh = load i64*, i64** %Sp_Var
  %ln1di = getelementptr inbounds i64, i64* %ln1dh, i32 2
  %ln1dj = ptrtoint i64* %ln1di to i64
  %ln1dk = inttoptr i64 %ln1dj to i64*
  store i64* %ln1dk, i64** %Sp_Var
  %ln1dl = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @c1bg_entry$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1dm = load i64*, i64** %Sp_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1dl( i64* %Base_Arg, i64* %ln1dm, i64* %Hp_Arg, i64 %R1_Arg, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@c1bg_entry = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @c1bg_entry$def to i8*)
define internal ghccc void @c1bg_entry$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind
{
c1bg:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %ln1dn = ptrtoint i8* @ghczmprim_GHCziTypes_True_closure to i64
  %ln1do = add i64 %ln1dn, 2
  store i64 %ln1do, i64* %R1_Var
  %ln1dp = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  %ln1dq = bitcast i64* %ln1dp to i64*
  %ln1dr = load i64, i64* %ln1dq, !tbaa !1
  %ln1ds = inttoptr i64 %ln1dr to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1dt = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1ds( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %ln1dt, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%Profile_progseq_closure_struct = type <{i64, i64}>
%c1eg_str_struct = type <{[2 x i8]}>
%c1eD_str_struct = type <{[6 x i8]}>
%c1eV_str_struct = type <{[7 x i8]}>
@Profile_progseq_closure$def = internal global %Profile_progseq_closure_struct<{i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Profile_progseq_info$def to i64), i64 0}>
@Profile_progseq_closure = alias i8* bitcast (%Profile_progseq_closure_struct* @Profile_progseq_closure$def to i8*)
@c1eg_str$def = internal constant %c1eg_str_struct<{[2 x i8] [i8 58, i8 0]}>
@c1eg_str = internal alias i8* bitcast (%c1eg_str_struct* @c1eg_str$def to i8*)
@c1eD_str$def = internal constant %c1eD_str_struct<{[6 x i8] [i8 83, i8 84, i8 79, i8 80, i8 32, i8 0]}>
@c1eD_str = internal alias i8* bitcast (%c1eD_str_struct* @c1eD_str$def to i8*)
@c1eV_str$def = internal constant %c1eV_str_struct<{[7 x i8] [i8 83, i8 84, i8 65, i8 82, i8 84, i8 32, i8 0]}>
@c1eV_str = internal alias i8* bitcast (%c1eV_str_struct* @c1eV_str$def to i8*)
@s17q_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @s17q_info$def to i8*)
define internal ghccc void @s17q_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 1, i64 17}>
{
c1e3:
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %ln1fb = load i64*, i64** %Sp_Var
  %ln1fc = getelementptr inbounds i64, i64* %ln1fb, i32 -3
  %ln1fd = ptrtoint i64* %ln1fc to i64
  %ln1fe = icmp ult i64 %ln1fd, %SpLim_Arg
  br i1 %ln1fe, label %c1e9, label %c1ea
c1ea:
  %ln1fg = ptrtoint i8* @stg_upd_frame_info to i64
  %ln1ff = load i64*, i64** %Sp_Var
  %ln1fh = getelementptr inbounds i64, i64* %ln1ff, i32 -2
  store i64 %ln1fg, i64* %ln1fh, !tbaa !1
  %ln1fj = load i64, i64* %R1_Var
  %ln1fi = load i64*, i64** %Sp_Var
  %ln1fk = getelementptr inbounds i64, i64* %ln1fi, i32 -1
  store i64 %ln1fj, i64* %ln1fk, !tbaa !1
  %ln1fm = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @c1e0_info$def to i64
  %ln1fl = load i64*, i64** %Sp_Var
  %ln1fn = getelementptr inbounds i64, i64* %ln1fl, i32 -3
  store i64 %ln1fm, i64* %ln1fn, !tbaa !1
  %ln1fq = load i64, i64* %R1_Var
  %ln1fr = add i64 %ln1fq, 16
  %ln1fs = inttoptr i64 %ln1fr to i64*
  %ln1ft = load i64, i64* %ln1fs, !tbaa !3
  store i64 %ln1ft, i64* %R1_Var
  %ln1fu = load i64*, i64** %Sp_Var
  %ln1fv = getelementptr inbounds i64, i64* %ln1fu, i32 -3
  %ln1fw = ptrtoint i64* %ln1fv to i64
  %ln1fx = inttoptr i64 %ln1fw to i64*
  store i64* %ln1fx, i64** %Sp_Var
  %ln1fy = load i64, i64* %R1_Var
  %ln1fz = and i64 %ln1fy, 7
  %ln1fA = icmp ne i64 %ln1fz, 0
  br i1 %ln1fA, label %u1ef, label %c1e1
c1e1:
  %ln1fC = load i64, i64* %R1_Var
  %ln1fD = inttoptr i64 %ln1fC to i64*
  %ln1fE = load i64, i64* %ln1fD, !tbaa !3
  %ln1fF = inttoptr i64 %ln1fE to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1fG = load i64*, i64** %Sp_Var
  %ln1fH = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1fF( i64* %Base_Arg, i64* %ln1fG, i64* %Hp_Arg, i64 %ln1fH, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
u1ef:
  %ln1fI = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @c1e0_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1fJ = load i64*, i64** %Sp_Var
  %ln1fK = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1fI( i64* %Base_Arg, i64* %ln1fJ, i64* %Hp_Arg, i64 %ln1fK, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
c1e9:
  %ln1fL = load i64, i64* %R1_Var
  store i64 %ln1fL, i64* %R1_Var
  %ln1fM = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %ln1fN = bitcast i64* %ln1fM to i64*
  %ln1fO = load i64, i64* %ln1fN, !tbaa !4
  %ln1fP = inttoptr i64 %ln1fO to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1fQ = load i64*, i64** %Sp_Var
  %ln1fR = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1fP( i64* %Base_Arg, i64* %ln1fQ, i64* %Hp_Arg, i64 %ln1fR, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@c1e0_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @c1e0_info$def to i8*)
define internal ghccc void @c1e0_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
c1e0:
  %R4_Var = alloca i64, i32 1
  store i64 undef, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %ln1fS = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @c1e6_info$def to i64
  %ln1fT = getelementptr inbounds i64, i64* %Sp_Arg, i32 0
  store i64 %ln1fS, i64* %ln1fT, !tbaa !1
  %ln1fU = ptrtoint i8* @ghczmprim_GHCziTypes_ZMZN_closure to i64
  %ln1fV = add i64 %ln1fU, 1
  store i64 %ln1fV, i64* %R4_Var
  %ln1fW = add i64 %R1_Arg, 7
  %ln1fX = inttoptr i64 %ln1fW to i64*
  %ln1fY = load i64, i64* %ln1fX, !tbaa !3
  store i64 %ln1fY, i64* %R3_Var
  store i64 0, i64* %R2_Var
  %ln1fZ = bitcast i8* @base_GHCziShow_zdwshowSignedInt_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1g0 = load i64, i64* %R2_Var
  %ln1g1 = load i64, i64* %R3_Var
  %ln1g2 = load i64, i64* %R4_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1fZ( i64* %Base_Arg, i64* %Sp_Arg, i64* %Hp_Arg, i64 %R1_Arg, i64 %ln1g0, i64 %ln1g1, i64 %ln1g2, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@c1e6_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @c1e6_info$def to i8*)
define internal ghccc void @c1e6_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 0, i64 32}>
{
c1e6:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %ln1g3 = load i64*, i64** %Hp_Var
  %ln1g4 = getelementptr inbounds i64, i64* %ln1g3, i32 3
  %ln1g5 = ptrtoint i64* %ln1g4 to i64
  %ln1g6 = inttoptr i64 %ln1g5 to i64*
  store i64* %ln1g6, i64** %Hp_Var
  %ln1g7 = load i64*, i64** %Hp_Var
  %ln1g8 = ptrtoint i64* %ln1g7 to i64
  %ln1g9 = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %ln1ga = bitcast i64* %ln1g9 to i64*
  %ln1gb = load i64, i64* %ln1ga, !tbaa !4
  %ln1gc = icmp ugt i64 %ln1g8, %ln1gb
  br i1 %ln1gc, label %c1ee, label %c1ed
c1ed:
  %ln1ge = ptrtoint i8* @ghczmprim_GHCziTypes_ZC_con_info to i64
  %ln1gd = load i64*, i64** %Hp_Var
  %ln1gf = getelementptr inbounds i64, i64* %ln1gd, i32 -2
  store i64 %ln1ge, i64* %ln1gf, !tbaa !2
  %ln1gh = load i64, i64* %R1_Var
  %ln1gg = load i64*, i64** %Hp_Var
  %ln1gi = getelementptr inbounds i64, i64* %ln1gg, i32 -1
  store i64 %ln1gh, i64* %ln1gi, !tbaa !2
  %ln1gk = load i64, i64* %R2_Var
  %ln1gj = load i64*, i64** %Hp_Var
  %ln1gl = getelementptr inbounds i64, i64* %ln1gj, i32 0
  store i64 %ln1gk, i64* %ln1gl, !tbaa !2
  %ln1gn = load i64*, i64** %Hp_Var
  %ln1go = ptrtoint i64* %ln1gn to i64
  %ln1gp = add i64 %ln1go, -14
  store i64 %ln1gp, i64* %R1_Var
  %ln1gq = load i64*, i64** %Sp_Var
  %ln1gr = getelementptr inbounds i64, i64* %ln1gq, i32 1
  %ln1gs = ptrtoint i64* %ln1gr to i64
  %ln1gt = inttoptr i64 %ln1gs to i64*
  store i64* %ln1gt, i64** %Sp_Var
  %ln1gu = load i64*, i64** %Sp_Var
  %ln1gv = getelementptr inbounds i64, i64* %ln1gu, i32 0
  %ln1gw = bitcast i64* %ln1gv to i64*
  %ln1gx = load i64, i64* %ln1gw, !tbaa !1
  %ln1gy = inttoptr i64 %ln1gx to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1gz = load i64*, i64** %Sp_Var
  %ln1gA = load i64*, i64** %Hp_Var
  %ln1gB = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1gy( i64* %Base_Arg, i64* %ln1gz, i64* %ln1gA, i64 %ln1gB, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
c1ee:
  %ln1gC = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %ln1gC, !tbaa !4
  %ln1gD = load i64, i64* %R2_Var
  store i64 %ln1gD, i64* %R2_Var
  %ln1gE = load i64, i64* %R1_Var
  store i64 %ln1gE, i64* %R1_Var
  %ln1gF = bitcast i8* @stg_gc_pp to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1gG = load i64*, i64** %Sp_Var
  %ln1gH = load i64*, i64** %Hp_Var
  %ln1gI = load i64, i64* %R1_Var
  %ln1gJ = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1gF( i64* %Base_Arg, i64* %ln1gG, i64* %ln1gH, i64 %ln1gI, i64 %ln1gJ, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@s17r_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @s17r_info$def to i8*)
define internal ghccc void @s17r_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 1, i64 17}>
{
c1eh:
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %ls17g = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %ln1gK = load i64*, i64** %Hp_Var
  %ln1gL = getelementptr inbounds i64, i64* %ln1gK, i32 3
  %ln1gM = ptrtoint i64* %ln1gL to i64
  %ln1gN = inttoptr i64 %ln1gM to i64*
  store i64* %ln1gN, i64** %Hp_Var
  %ln1gO = load i64*, i64** %Hp_Var
  %ln1gP = ptrtoint i64* %ln1gO to i64
  %ln1gQ = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %ln1gR = bitcast i64* %ln1gQ to i64*
  %ln1gS = load i64, i64* %ln1gR, !tbaa !4
  %ln1gT = icmp ugt i64 %ln1gP, %ln1gS
  br i1 %ln1gT, label %c1el, label %c1ek
c1ek:
  %ln1gW = load i64, i64* %R1_Var
  %ln1gX = add i64 %ln1gW, 16
  %ln1gY = inttoptr i64 %ln1gX to i64*
  %ln1gZ = load i64, i64* %ln1gY, !tbaa !3
  store i64 %ln1gZ, i64* %ls17g
  %ln1h1 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @s17q_info$def to i64
  %ln1h0 = load i64*, i64** %Hp_Var
  %ln1h2 = getelementptr inbounds i64, i64* %ln1h0, i32 -2
  store i64 %ln1h1, i64* %ln1h2, !tbaa !2
  %ln1h4 = load i64, i64* %ls17g
  %ln1h3 = load i64*, i64** %Hp_Var
  %ln1h5 = getelementptr inbounds i64, i64* %ln1h3, i32 0
  store i64 %ln1h4, i64* %ln1h5, !tbaa !2
  %ln1h6 = load i64*, i64** %Hp_Var
  %ln1h7 = getelementptr inbounds i64, i64* %ln1h6, i32 -2
  %ln1h8 = ptrtoint i64* %ln1h7 to i64
  store i64 %ln1h8, i64* %R3_Var
  %ln1h9 = ptrtoint %c1eg_str_struct* @c1eg_str$def to i64
  store i64 %ln1h9, i64* %R2_Var
  %ln1ha = bitcast i8* @ghczmprim_GHCziCString_unpackAppendCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1hb = load i64*, i64** %Hp_Var
  %ln1hc = load i64, i64* %R1_Var
  %ln1hd = load i64, i64* %R2_Var
  %ln1he = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1ha( i64* %Base_Arg, i64* %Sp_Arg, i64* %ln1hb, i64 %ln1hc, i64 %ln1hd, i64 %ln1he, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
c1el:
  %ln1hf = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %ln1hf, !tbaa !4
  %ln1hg = load i64, i64* %R1_Var
  store i64 %ln1hg, i64* %R1_Var
  %ln1hh = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %ln1hi = bitcast i64* %ln1hh to i64*
  %ln1hj = load i64, i64* %ln1hi, !tbaa !4
  %ln1hk = inttoptr i64 %ln1hj to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1hl = load i64*, i64** %Hp_Var
  %ln1hm = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1hk( i64* %Base_Arg, i64* %Sp_Arg, i64* %ln1hl, i64 %ln1hm, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@s17k_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @s17k_info$def to i8*)
define internal ghccc void @s17k_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 2, i64 19}>
{
c1em:
  %ls17k = alloca i64, i32 1
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %ls17f = alloca i64, i32 1
  %ls17g = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %ln1hn = load i64, i64* %R1_Var
  store i64 %ln1hn, i64* %ls17k
  %ln1ho = load i64*, i64** %Sp_Var
  %ln1hp = getelementptr inbounds i64, i64* %ln1ho, i32 -2
  %ln1hq = ptrtoint i64* %ln1hp to i64
  %ln1hr = icmp ult i64 %ln1hq, %SpLim_Arg
  br i1 %ln1hr, label %c1en, label %c1eo
c1eo:
  %ln1hs = load i64*, i64** %Hp_Var
  %ln1ht = getelementptr inbounds i64, i64* %ln1hs, i32 3
  %ln1hu = ptrtoint i64* %ln1ht to i64
  %ln1hv = inttoptr i64 %ln1hu to i64*
  store i64* %ln1hv, i64** %Hp_Var
  %ln1hw = load i64*, i64** %Hp_Var
  %ln1hx = ptrtoint i64* %ln1hw to i64
  %ln1hy = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %ln1hz = bitcast i64* %ln1hy to i64*
  %ln1hA = load i64, i64* %ln1hz, !tbaa !4
  %ln1hB = icmp ugt i64 %ln1hx, %ln1hA
  br i1 %ln1hB, label %c1eq, label %c1ep
c1ep:
  %ln1hD = ptrtoint i8* @stg_upd_frame_info to i64
  %ln1hC = load i64*, i64** %Sp_Var
  %ln1hE = getelementptr inbounds i64, i64* %ln1hC, i32 -2
  store i64 %ln1hD, i64* %ln1hE, !tbaa !1
  %ln1hG = load i64, i64* %ls17k
  %ln1hF = load i64*, i64** %Sp_Var
  %ln1hH = getelementptr inbounds i64, i64* %ln1hF, i32 -1
  store i64 %ln1hG, i64* %ln1hH, !tbaa !1
  %ln1hI = load i64, i64* %ls17k
  %ln1hJ = add i64 %ln1hI, 16
  %ln1hK = inttoptr i64 %ln1hJ to i64*
  %ln1hL = load i64, i64* %ln1hK, !tbaa !5
  store i64 %ln1hL, i64* %ls17f
  %ln1hM = load i64, i64* %ls17k
  %ln1hN = add i64 %ln1hM, 24
  %ln1hO = inttoptr i64 %ln1hN to i64*
  %ln1hP = load i64, i64* %ln1hO, !tbaa !5
  store i64 %ln1hP, i64* %ls17g
  %ln1hR = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @s17r_info$def to i64
  %ln1hQ = load i64*, i64** %Hp_Var
  %ln1hS = getelementptr inbounds i64, i64* %ln1hQ, i32 -2
  store i64 %ln1hR, i64* %ln1hS, !tbaa !2
  %ln1hU = load i64, i64* %ls17g
  %ln1hT = load i64*, i64** %Hp_Var
  %ln1hV = getelementptr inbounds i64, i64* %ln1hT, i32 0
  store i64 %ln1hU, i64* %ln1hV, !tbaa !2
  %ln1hW = load i64*, i64** %Hp_Var
  %ln1hX = getelementptr inbounds i64, i64* %ln1hW, i32 -2
  %ln1hY = ptrtoint i64* %ln1hX to i64
  store i64 %ln1hY, i64* %R3_Var
  %ln1hZ = load i64, i64* %ls17f
  store i64 %ln1hZ, i64* %R2_Var
  %ln1i0 = load i64*, i64** %Sp_Var
  %ln1i1 = getelementptr inbounds i64, i64* %ln1i0, i32 -2
  %ln1i2 = ptrtoint i64* %ln1i1 to i64
  %ln1i3 = inttoptr i64 %ln1i2 to i64*
  store i64* %ln1i3, i64** %Sp_Var
  %ln1i4 = bitcast i8* @base_GHCziBase_zpzp_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1i5 = load i64*, i64** %Sp_Var
  %ln1i6 = load i64*, i64** %Hp_Var
  %ln1i7 = load i64, i64* %R1_Var
  %ln1i8 = load i64, i64* %R2_Var
  %ln1i9 = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1i4( i64* %Base_Arg, i64* %ln1i5, i64* %ln1i6, i64 %ln1i7, i64 %ln1i8, i64 %ln1i9, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
c1eq:
  %ln1ia = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %ln1ia, !tbaa !4
  br label %c1en
c1en:
  %ln1ib = load i64, i64* %ls17k
  store i64 %ln1ib, i64* %R1_Var
  %ln1ic = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %ln1id = bitcast i64* %ln1ic to i64*
  %ln1ie = load i64, i64* %ln1id, !tbaa !4
  %ln1if = inttoptr i64 %ln1ie to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1ig = load i64*, i64** %Sp_Var
  %ln1ih = load i64*, i64** %Hp_Var
  %ln1ii = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1if( i64* %Base_Arg, i64* %ln1ig, i64* %ln1ih, i64 %ln1ii, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@s17t_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @s17t_info$def to i8*)
define internal ghccc void @s17t_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 1, i64 17}>
{
c1eE:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %ln1ij = load i64*, i64** %Sp_Var
  %ln1ik = getelementptr inbounds i64, i64* %ln1ij, i32 -2
  %ln1il = ptrtoint i64* %ln1ik to i64
  %ln1im = icmp ult i64 %ln1il, %SpLim_Arg
  br i1 %ln1im, label %c1eF, label %c1eG
c1eG:
  %ln1io = ptrtoint i8* @stg_upd_frame_info to i64
  %ln1in = load i64*, i64** %Sp_Var
  %ln1ip = getelementptr inbounds i64, i64* %ln1in, i32 -2
  store i64 %ln1io, i64* %ln1ip, !tbaa !1
  %ln1ir = load i64, i64* %R1_Var
  %ln1iq = load i64*, i64** %Sp_Var
  %ln1is = getelementptr inbounds i64, i64* %ln1iq, i32 -1
  store i64 %ln1ir, i64* %ln1is, !tbaa !1
  %ln1iv = load i64, i64* %R1_Var
  %ln1iw = add i64 %ln1iv, 16
  %ln1ix = inttoptr i64 %ln1iw to i64*
  %ln1iy = load i64, i64* %ln1ix, !tbaa !3
  store i64 %ln1iy, i64* %R3_Var
  %ln1iz = ptrtoint %c1eD_str_struct* @c1eD_str$def to i64
  store i64 %ln1iz, i64* %R2_Var
  %ln1iA = load i64*, i64** %Sp_Var
  %ln1iB = getelementptr inbounds i64, i64* %ln1iA, i32 -2
  %ln1iC = ptrtoint i64* %ln1iB to i64
  %ln1iD = inttoptr i64 %ln1iC to i64*
  store i64* %ln1iD, i64** %Sp_Var
  %ln1iE = bitcast i8* @ghczmprim_GHCziCString_unpackAppendCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1iF = load i64*, i64** %Sp_Var
  %ln1iG = load i64, i64* %R1_Var
  %ln1iH = load i64, i64* %R2_Var
  %ln1iI = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1iE( i64* %Base_Arg, i64* %ln1iF, i64* %Hp_Arg, i64 %ln1iG, i64 %ln1iH, i64 %ln1iI, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
c1eF:
  %ln1iJ = load i64, i64* %R1_Var
  store i64 %ln1iJ, i64* %R1_Var
  %ln1iK = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %ln1iL = bitcast i64* %ln1iK to i64*
  %ln1iM = load i64, i64* %ln1iL, !tbaa !4
  %ln1iN = inttoptr i64 %ln1iM to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1iO = load i64*, i64** %Sp_Var
  %ln1iP = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1iN( i64* %Base_Arg, i64* %ln1iO, i64* %Hp_Arg, i64 %ln1iP, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@s17u_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @s17u_info$def to i8*)
define internal ghccc void @s17u_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @S1bV_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @s17u_info$def to i64)),i64 40), i64 2, i64 4294967315}>
{
c1eH:
  %ls17u = alloca i64, i32 1
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %ls17i = alloca i64, i32 1
  %ls17k = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %ln1iQ = load i64, i64* %R1_Var
  store i64 %ln1iQ, i64* %ls17u
  %ln1iR = load i64*, i64** %Sp_Var
  %ln1iS = getelementptr inbounds i64, i64* %ln1iR, i32 -2
  %ln1iT = ptrtoint i64* %ln1iS to i64
  %ln1iU = icmp ult i64 %ln1iT, %SpLim_Arg
  br i1 %ln1iU, label %c1eI, label %c1eJ
c1eJ:
  %ln1iV = load i64*, i64** %Hp_Var
  %ln1iW = getelementptr inbounds i64, i64* %ln1iV, i32 3
  %ln1iX = ptrtoint i64* %ln1iW to i64
  %ln1iY = inttoptr i64 %ln1iX to i64*
  store i64* %ln1iY, i64** %Hp_Var
  %ln1iZ = load i64*, i64** %Hp_Var
  %ln1j0 = ptrtoint i64* %ln1iZ to i64
  %ln1j1 = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %ln1j2 = bitcast i64* %ln1j1 to i64*
  %ln1j3 = load i64, i64* %ln1j2, !tbaa !4
  %ln1j4 = icmp ugt i64 %ln1j0, %ln1j3
  br i1 %ln1j4, label %c1eL, label %c1eK
c1eK:
  %ln1j6 = ptrtoint i8* @stg_upd_frame_info to i64
  %ln1j5 = load i64*, i64** %Sp_Var
  %ln1j7 = getelementptr inbounds i64, i64* %ln1j5, i32 -2
  store i64 %ln1j6, i64* %ln1j7, !tbaa !1
  %ln1j9 = load i64, i64* %ls17u
  %ln1j8 = load i64*, i64** %Sp_Var
  %ln1ja = getelementptr inbounds i64, i64* %ln1j8, i32 -1
  store i64 %ln1j9, i64* %ln1ja, !tbaa !1
  %ln1jb = load i64, i64* %ls17u
  %ln1jc = add i64 %ln1jb, 16
  %ln1jd = inttoptr i64 %ln1jc to i64*
  %ln1je = load i64, i64* %ln1jd, !tbaa !5
  store i64 %ln1je, i64* %ls17i
  %ln1jf = load i64, i64* %ls17u
  %ln1jg = add i64 %ln1jf, 24
  %ln1jh = inttoptr i64 %ln1jg to i64*
  %ln1ji = load i64, i64* %ln1jh, !tbaa !5
  store i64 %ln1ji, i64* %ls17k
  %ln1jk = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @s17t_info$def to i64
  %ln1jj = load i64*, i64** %Hp_Var
  %ln1jl = getelementptr inbounds i64, i64* %ln1jj, i32 -2
  store i64 %ln1jk, i64* %ln1jl, !tbaa !2
  %ln1jn = load i64, i64* %ls17k
  %ln1jm = load i64*, i64** %Hp_Var
  %ln1jo = getelementptr inbounds i64, i64* %ln1jm, i32 0
  store i64 %ln1jn, i64* %ln1jo, !tbaa !2
  %ln1jp = load i64, i64* %ls17i
  store i64 %ln1jp, i64* %R3_Var
  %ln1jq = load i64*, i64** %Hp_Var
  %ln1jr = getelementptr inbounds i64, i64* %ln1jq, i32 -2
  %ln1js = ptrtoint i64* %ln1jr to i64
  store i64 %ln1js, i64* %R2_Var
  %ln1jt = load i64*, i64** %Sp_Var
  %ln1ju = getelementptr inbounds i64, i64* %ln1jt, i32 -2
  %ln1jv = ptrtoint i64* %ln1ju to i64
  %ln1jw = inttoptr i64 %ln1jv to i64*
  store i64* %ln1jw, i64** %Sp_Var
  %ln1jx = bitcast i8* @base_DebugziTrace_traceEvent_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1jy = load i64*, i64** %Sp_Var
  %ln1jz = load i64*, i64** %Hp_Var
  %ln1jA = load i64, i64* %R1_Var
  %ln1jB = load i64, i64* %R2_Var
  %ln1jC = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1jx( i64* %Base_Arg, i64* %ln1jy, i64* %ln1jz, i64 %ln1jA, i64 %ln1jB, i64 %ln1jC, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
c1eL:
  %ln1jD = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 24, i64* %ln1jD, !tbaa !4
  br label %c1eI
c1eI:
  %ln1jE = load i64, i64* %ls17u
  store i64 %ln1jE, i64* %R1_Var
  %ln1jF = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %ln1jG = bitcast i64* %ln1jF to i64*
  %ln1jH = load i64, i64* %ln1jG, !tbaa !4
  %ln1jI = inttoptr i64 %ln1jH to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1jJ = load i64*, i64** %Sp_Var
  %ln1jK = load i64*, i64** %Hp_Var
  %ln1jL = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1jI( i64* %Base_Arg, i64* %ln1jJ, i64* %ln1jK, i64 %ln1jL, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@s17v_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @s17v_info$def to i8*)
define internal ghccc void @s17v_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @S1bV_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @s17v_info$def to i64)),i64 40), i64 3, i64 4294967312}>
{
c1eM:
  %ls17v = alloca i64, i32 1
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %ls17h = alloca i64, i32 1
  %ls17i = alloca i64, i32 1
  %ls17k = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %ln1jM = load i64, i64* %R1_Var
  store i64 %ln1jM, i64* %ls17v
  %ln1jN = load i64*, i64** %Sp_Var
  %ln1jO = getelementptr inbounds i64, i64* %ln1jN, i32 -2
  %ln1jP = ptrtoint i64* %ln1jO to i64
  %ln1jQ = icmp ult i64 %ln1jP, %SpLim_Arg
  br i1 %ln1jQ, label %c1eN, label %c1eO
c1eO:
  %ln1jR = load i64*, i64** %Hp_Var
  %ln1jS = getelementptr inbounds i64, i64* %ln1jR, i32 4
  %ln1jT = ptrtoint i64* %ln1jS to i64
  %ln1jU = inttoptr i64 %ln1jT to i64*
  store i64* %ln1jU, i64** %Hp_Var
  %ln1jV = load i64*, i64** %Hp_Var
  %ln1jW = ptrtoint i64* %ln1jV to i64
  %ln1jX = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %ln1jY = bitcast i64* %ln1jX to i64*
  %ln1jZ = load i64, i64* %ln1jY, !tbaa !4
  %ln1k0 = icmp ugt i64 %ln1jW, %ln1jZ
  br i1 %ln1k0, label %c1eQ, label %c1eP
c1eP:
  %ln1k2 = ptrtoint i8* @stg_upd_frame_info to i64
  %ln1k1 = load i64*, i64** %Sp_Var
  %ln1k3 = getelementptr inbounds i64, i64* %ln1k1, i32 -2
  store i64 %ln1k2, i64* %ln1k3, !tbaa !1
  %ln1k5 = load i64, i64* %ls17v
  %ln1k4 = load i64*, i64** %Sp_Var
  %ln1k6 = getelementptr inbounds i64, i64* %ln1k4, i32 -1
  store i64 %ln1k5, i64* %ln1k6, !tbaa !1
  %ln1k7 = load i64, i64* %ls17v
  %ln1k8 = add i64 %ln1k7, 16
  %ln1k9 = inttoptr i64 %ln1k8 to i64*
  %ln1ka = load i64, i64* %ln1k9, !tbaa !5
  store i64 %ln1ka, i64* %ls17h
  %ln1kb = load i64, i64* %ls17v
  %ln1kc = add i64 %ln1kb, 24
  %ln1kd = inttoptr i64 %ln1kc to i64*
  %ln1ke = load i64, i64* %ln1kd, !tbaa !5
  store i64 %ln1ke, i64* %ls17i
  %ln1kf = load i64, i64* %ls17v
  %ln1kg = add i64 %ln1kf, 32
  %ln1kh = inttoptr i64 %ln1kg to i64*
  %ln1ki = load i64, i64* %ln1kh, !tbaa !5
  store i64 %ln1ki, i64* %ls17k
  %ln1kk = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @s17u_info$def to i64
  %ln1kj = load i64*, i64** %Hp_Var
  %ln1kl = getelementptr inbounds i64, i64* %ln1kj, i32 -3
  store i64 %ln1kk, i64* %ln1kl, !tbaa !2
  %ln1kn = load i64, i64* %ls17i
  %ln1km = load i64*, i64** %Hp_Var
  %ln1ko = getelementptr inbounds i64, i64* %ln1km, i32 -1
  store i64 %ln1kn, i64* %ln1ko, !tbaa !2
  %ln1kq = load i64, i64* %ls17k
  %ln1kp = load i64*, i64** %Hp_Var
  %ln1kr = getelementptr inbounds i64, i64* %ln1kp, i32 0
  store i64 %ln1kq, i64* %ln1kr, !tbaa !2
  %ln1ks = load i64*, i64** %Hp_Var
  %ln1kt = getelementptr inbounds i64, i64* %ln1ks, i32 -3
  %ln1ku = ptrtoint i64* %ln1kt to i64
  store i64 %ln1ku, i64* %R2_Var
  %ln1kv = load i64, i64* %ls17h
  store i64 %ln1kv, i64* %R1_Var
  %ln1kw = load i64*, i64** %Sp_Var
  %ln1kx = getelementptr inbounds i64, i64* %ln1kw, i32 -2
  %ln1ky = ptrtoint i64* %ln1kx to i64
  %ln1kz = inttoptr i64 %ln1ky to i64*
  store i64* %ln1kz, i64** %Sp_Var
  %ln1kA = bitcast i8* @stg_ap_p_fast to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1kB = load i64*, i64** %Sp_Var
  %ln1kC = load i64*, i64** %Hp_Var
  %ln1kD = load i64, i64* %R1_Var
  %ln1kE = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1kA( i64* %Base_Arg, i64* %ln1kB, i64* %ln1kC, i64 %ln1kD, i64 %ln1kE, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
c1eQ:
  %ln1kF = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 32, i64* %ln1kF, !tbaa !4
  br label %c1eN
c1eN:
  %ln1kG = load i64, i64* %ls17v
  store i64 %ln1kG, i64* %R1_Var
  %ln1kH = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %ln1kI = bitcast i64* %ln1kH to i64*
  %ln1kJ = load i64, i64* %ln1kI, !tbaa !4
  %ln1kK = inttoptr i64 %ln1kJ to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1kL = load i64*, i64** %Sp_Var
  %ln1kM = load i64*, i64** %Hp_Var
  %ln1kN = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1kK( i64* %Base_Arg, i64* %ln1kL, i64* %ln1kM, i64 %ln1kN, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@s17s_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @s17s_info$def to i8*)
define internal ghccc void @s17s_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64}><{i64 1, i64 17}>
{
c1eW:
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %ln1kO = load i64*, i64** %Sp_Var
  %ln1kP = getelementptr inbounds i64, i64* %ln1kO, i32 -2
  %ln1kQ = ptrtoint i64* %ln1kP to i64
  %ln1kR = icmp ult i64 %ln1kQ, %SpLim_Arg
  br i1 %ln1kR, label %c1eX, label %c1eY
c1eY:
  %ln1kT = ptrtoint i8* @stg_upd_frame_info to i64
  %ln1kS = load i64*, i64** %Sp_Var
  %ln1kU = getelementptr inbounds i64, i64* %ln1kS, i32 -2
  store i64 %ln1kT, i64* %ln1kU, !tbaa !1
  %ln1kW = load i64, i64* %R1_Var
  %ln1kV = load i64*, i64** %Sp_Var
  %ln1kX = getelementptr inbounds i64, i64* %ln1kV, i32 -1
  store i64 %ln1kW, i64* %ln1kX, !tbaa !1
  %ln1l0 = load i64, i64* %R1_Var
  %ln1l1 = add i64 %ln1l0, 16
  %ln1l2 = inttoptr i64 %ln1l1 to i64*
  %ln1l3 = load i64, i64* %ln1l2, !tbaa !3
  store i64 %ln1l3, i64* %R3_Var
  %ln1l4 = ptrtoint %c1eV_str_struct* @c1eV_str$def to i64
  store i64 %ln1l4, i64* %R2_Var
  %ln1l5 = load i64*, i64** %Sp_Var
  %ln1l6 = getelementptr inbounds i64, i64* %ln1l5, i32 -2
  %ln1l7 = ptrtoint i64* %ln1l6 to i64
  %ln1l8 = inttoptr i64 %ln1l7 to i64*
  store i64* %ln1l8, i64** %Sp_Var
  %ln1l9 = bitcast i8* @ghczmprim_GHCziCString_unpackAppendCStringzh_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1la = load i64*, i64** %Sp_Var
  %ln1lb = load i64, i64* %R1_Var
  %ln1lc = load i64, i64* %R2_Var
  %ln1ld = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1l9( i64* %Base_Arg, i64* %ln1la, i64* %Hp_Arg, i64 %ln1lb, i64 %ln1lc, i64 %ln1ld, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
c1eX:
  %ln1le = load i64, i64* %R1_Var
  store i64 %ln1le, i64* %R1_Var
  %ln1lf = getelementptr inbounds i64, i64* %Base_Arg, i32 -2
  %ln1lg = bitcast i64* %ln1lf to i64*
  %ln1lh = load i64, i64* %ln1lg, !tbaa !4
  %ln1li = inttoptr i64 %ln1lh to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1lj = load i64*, i64** %Sp_Var
  %ln1lk = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1li( i64* %Base_Arg, i64* %ln1lj, i64* %Hp_Arg, i64 %ln1lk, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@Profile_progseq_info = alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Profile_progseq_info$def to i8*)
define ghccc void @Profile_progseq_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @S1bV_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Profile_progseq_info$def to i64)),i64 32), i64 17179869208, i64 0, i64 30064771087}>
{
c1f3:
  %ls17f = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 %R2_Arg, i64* %R2_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %R5_Var = alloca i64, i32 1
  store i64 %R5_Arg, i64* %R5_Var
  %R4_Var = alloca i64, i32 1
  store i64 %R4_Arg, i64* %R4_Var
  %R3_Var = alloca i64, i32 1
  store i64 %R3_Arg, i64* %R3_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %ln1ll = load i64*, i64** %Sp_Var
  %ln1lm = getelementptr inbounds i64, i64* %ln1ll, i32 -5
  %ln1ln = ptrtoint i64* %ln1lm to i64
  %ln1lo = icmp ult i64 %ln1ln, %SpLim_Arg
  br i1 %ln1lo, label %c1f4, label %c1f5
c1f5:
  %ln1lq = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @c1dJ_info$def to i64
  %ln1lp = load i64*, i64** %Sp_Var
  %ln1lr = getelementptr inbounds i64, i64* %ln1lp, i32 -5
  store i64 %ln1lq, i64* %ln1lr, !tbaa !1
  %ln1ls = load i64, i64* %R2_Var
  store i64 %ln1ls, i64* %ls17f
  %ln1lt = load i64, i64* %R2_Var
  store i64 %ln1lt, i64* %R2_Var
  %ln1lv = load i64, i64* %ls17f
  %ln1lu = load i64*, i64** %Sp_Var
  %ln1lw = getelementptr inbounds i64, i64* %ln1lu, i32 -4
  store i64 %ln1lv, i64* %ln1lw, !tbaa !1
  %ln1ly = load i64, i64* %R3_Var
  %ln1lx = load i64*, i64** %Sp_Var
  %ln1lz = getelementptr inbounds i64, i64* %ln1lx, i32 -3
  store i64 %ln1ly, i64* %ln1lz, !tbaa !1
  %ln1lB = load i64, i64* %R4_Var
  %ln1lA = load i64*, i64** %Sp_Var
  %ln1lC = getelementptr inbounds i64, i64* %ln1lA, i32 -2
  store i64 %ln1lB, i64* %ln1lC, !tbaa !1
  %ln1lE = load i64, i64* %R5_Var
  %ln1lD = load i64*, i64** %Sp_Var
  %ln1lF = getelementptr inbounds i64, i64* %ln1lD, i32 -1
  store i64 %ln1lE, i64* %ln1lF, !tbaa !1
  %ln1lG = load i64*, i64** %Sp_Var
  %ln1lH = getelementptr inbounds i64, i64* %ln1lG, i32 -5
  %ln1lI = ptrtoint i64* %ln1lH to i64
  %ln1lJ = inttoptr i64 %ln1lI to i64*
  store i64* %ln1lJ, i64** %Sp_Var
  %ln1lK = bitcast void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @Profile_doTraceFile_info$def to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1lL = load i64*, i64** %Sp_Var
  %ln1lM = load i64, i64* %R1_Var
  %ln1lN = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1lK( i64* %Base_Arg, i64* %ln1lL, i64* %Hp_Arg, i64 %ln1lM, i64 %ln1lN, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
c1f4:
  %ln1lO = load i64, i64* %R5_Var
  store i64 %ln1lO, i64* %R5_Var
  %ln1lP = load i64, i64* %R4_Var
  store i64 %ln1lP, i64* %R4_Var
  %ln1lQ = load i64, i64* %R3_Var
  store i64 %ln1lQ, i64* %R3_Var
  %ln1lR = load i64, i64* %R2_Var
  store i64 %ln1lR, i64* %R2_Var
  %ln1lS = ptrtoint %Profile_progseq_closure_struct* @Profile_progseq_closure$def to i64
  store i64 %ln1lS, i64* %R1_Var
  %ln1lT = getelementptr inbounds i64, i64* %Base_Arg, i32 -1
  %ln1lU = bitcast i64* %ln1lT to i64*
  %ln1lV = load i64, i64* %ln1lU, !tbaa !4
  %ln1lW = inttoptr i64 %ln1lV to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1lX = load i64*, i64** %Sp_Var
  %ln1lY = load i64, i64* %R1_Var
  %ln1lZ = load i64, i64* %R2_Var
  %ln1m0 = load i64, i64* %R3_Var
  %ln1m1 = load i64, i64* %R4_Var
  %ln1m2 = load i64, i64* %R5_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1lW( i64* %Base_Arg, i64* %ln1lX, i64* %Hp_Arg, i64 %ln1lY, i64 %ln1lZ, i64 %ln1m0, i64 %ln1m1, i64 %ln1m2, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
@c1dJ_info = internal alias i8* bitcast (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @c1dJ_info$def to i8*)
define internal ghccc void @c1dJ_info$def(i64* noalias nocapture %Base_Arg, i64* noalias nocapture %Sp_Arg, i64* noalias nocapture %Hp_Arg, i64 %R1_Arg, i64 %R2_Arg, i64 %R3_Arg, i64 %R4_Arg, i64 %R5_Arg, i64 %R6_Arg, i64 %SpLim_Arg) align 8 nounwind prefix <{i64, i64, i64}><{i64 add (i64 sub (i64 ptrtoint (i8* @S1bV_srt to i64),i64 ptrtoint (void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @c1dJ_info$def to i64)),i64 40), i64 4, i64 4294967328}>
{
c1dJ:
  %ls17h = alloca i64, i32 1
  %ls17i = alloca i64, i32 1
  %R2_Var = alloca i64, i32 1
  store i64 undef, i64* %R2_Var
  %R1_Var = alloca i64, i32 1
  store i64 %R1_Arg, i64* %R1_Var
  %Sp_Var = alloca i64*, i32 1
  store i64* %Sp_Arg, i64** %Sp_Var
  %Hp_Var = alloca i64*, i32 1
  store i64* %Hp_Arg, i64** %Hp_Var
  %lc1dO = alloca i64, i32 1
  %R3_Var = alloca i64, i32 1
  store i64 undef, i64* %R3_Var
  %ln1m3 = load i64*, i64** %Sp_Var
  %ln1m4 = getelementptr inbounds i64, i64* %ln1m3, i32 3
  %ln1m5 = bitcast i64* %ln1m4 to i64*
  %ln1m6 = load i64, i64* %ln1m5, !tbaa !1
  store i64 %ln1m6, i64* %ls17h
  %ln1m7 = load i64*, i64** %Sp_Var
  %ln1m8 = getelementptr inbounds i64, i64* %ln1m7, i32 4
  %ln1m9 = bitcast i64* %ln1m8 to i64*
  %ln1ma = load i64, i64* %ln1m9, !tbaa !1
  store i64 %ln1ma, i64* %ls17i
  %ln1mb = load i64, i64* %R1_Var
  %ln1mc = and i64 %ln1mb, 7
  switch i64 %ln1mc, label %c1f0 [i64 1, label %c1f0
i64 2, label %c1f1]
c1f0:
  %ln1md = load i64, i64* %ls17i
  store i64 %ln1md, i64* %R2_Var
  %ln1me = load i64, i64* %ls17h
  store i64 %ln1me, i64* %R1_Var
  %ln1mf = load i64*, i64** %Sp_Var
  %ln1mg = getelementptr inbounds i64, i64* %ln1mf, i32 5
  %ln1mh = ptrtoint i64* %ln1mg to i64
  %ln1mi = inttoptr i64 %ln1mh to i64*
  store i64* %ln1mi, i64** %Sp_Var
  %ln1mj = bitcast i8* @stg_ap_p_fast to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1mk = load i64*, i64** %Sp_Var
  %ln1ml = load i64*, i64** %Hp_Var
  %ln1mm = load i64, i64* %R1_Var
  %ln1mn = load i64, i64* %R2_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1mj( i64* %Base_Arg, i64* %ln1mk, i64* %ln1ml, i64 %ln1mm, i64 %ln1mn, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
c1f1:
  %ln1mo = load i64*, i64** %Hp_Var
  %ln1mp = getelementptr inbounds i64, i64* %ln1mo, i32 12
  %ln1mq = ptrtoint i64* %ln1mp to i64
  %ln1mr = inttoptr i64 %ln1mq to i64*
  store i64* %ln1mr, i64** %Hp_Var
  %ln1ms = load i64*, i64** %Hp_Var
  %ln1mt = ptrtoint i64* %ln1ms to i64
  %ln1mu = getelementptr inbounds i64, i64* %Base_Arg, i32 107
  %ln1mv = bitcast i64* %ln1mu to i64*
  %ln1mw = load i64, i64* %ln1mv, !tbaa !4
  %ln1mx = icmp ugt i64 %ln1mt, %ln1mw
  br i1 %ln1mx, label %c1fa, label %c1f9
c1f9:
  %ln1mz = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @s17k_info$def to i64
  %ln1my = load i64*, i64** %Hp_Var
  %ln1mA = getelementptr inbounds i64, i64* %ln1my, i32 -11
  store i64 %ln1mz, i64* %ln1mA, !tbaa !2
  %ln1mC = load i64*, i64** %Sp_Var
  %ln1mD = getelementptr inbounds i64, i64* %ln1mC, i32 1
  %ln1mE = bitcast i64* %ln1mD to i64*
  %ln1mF = load i64, i64* %ln1mE, !tbaa !1
  %ln1mB = load i64*, i64** %Hp_Var
  %ln1mG = getelementptr inbounds i64, i64* %ln1mB, i32 -9
  store i64 %ln1mF, i64* %ln1mG, !tbaa !2
  %ln1mI = load i64*, i64** %Sp_Var
  %ln1mJ = getelementptr inbounds i64, i64* %ln1mI, i32 2
  %ln1mK = bitcast i64* %ln1mJ to i64*
  %ln1mL = load i64, i64* %ln1mK, !tbaa !1
  %ln1mH = load i64*, i64** %Hp_Var
  %ln1mM = getelementptr inbounds i64, i64* %ln1mH, i32 -8
  store i64 %ln1mL, i64* %ln1mM, !tbaa !2
  %ln1mO = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @s17v_info$def to i64
  %ln1mN = load i64*, i64** %Hp_Var
  %ln1mP = getelementptr inbounds i64, i64* %ln1mN, i32 -7
  store i64 %ln1mO, i64* %ln1mP, !tbaa !2
  %ln1mR = load i64, i64* %ls17h
  %ln1mQ = load i64*, i64** %Hp_Var
  %ln1mS = getelementptr inbounds i64, i64* %ln1mQ, i32 -5
  store i64 %ln1mR, i64* %ln1mS, !tbaa !2
  %ln1mU = load i64, i64* %ls17i
  %ln1mT = load i64*, i64** %Hp_Var
  %ln1mV = getelementptr inbounds i64, i64* %ln1mT, i32 -4
  store i64 %ln1mU, i64* %ln1mV, !tbaa !2
  %ln1mW = load i64*, i64** %Hp_Var
  %ln1mX = getelementptr inbounds i64, i64* %ln1mW, i32 -11
  %ln1mY = ptrtoint i64* %ln1mX to i64
  store i64 %ln1mY, i64* %lc1dO
  %ln1n0 = load i64, i64* %lc1dO
  %ln1mZ = load i64*, i64** %Hp_Var
  %ln1n1 = getelementptr inbounds i64, i64* %ln1mZ, i32 -3
  store i64 %ln1n0, i64* %ln1n1, !tbaa !2
  %ln1n3 = ptrtoint void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)* @s17s_info$def to i64
  %ln1n2 = load i64*, i64** %Hp_Var
  %ln1n4 = getelementptr inbounds i64, i64* %ln1n2, i32 -2
  store i64 %ln1n3, i64* %ln1n4, !tbaa !2
  %ln1n6 = load i64, i64* %lc1dO
  %ln1n5 = load i64*, i64** %Hp_Var
  %ln1n7 = getelementptr inbounds i64, i64* %ln1n5, i32 0
  store i64 %ln1n6, i64* %ln1n7, !tbaa !2
  %ln1n8 = load i64*, i64** %Hp_Var
  %ln1n9 = getelementptr inbounds i64, i64* %ln1n8, i32 -7
  %ln1na = ptrtoint i64* %ln1n9 to i64
  store i64 %ln1na, i64* %R3_Var
  %ln1nb = load i64*, i64** %Hp_Var
  %ln1nc = getelementptr inbounds i64, i64* %ln1nb, i32 -2
  %ln1nd = ptrtoint i64* %ln1nc to i64
  store i64 %ln1nd, i64* %R2_Var
  %ln1ne = load i64*, i64** %Sp_Var
  %ln1nf = getelementptr inbounds i64, i64* %ln1ne, i32 5
  %ln1ng = ptrtoint i64* %ln1nf to i64
  %ln1nh = inttoptr i64 %ln1ng to i64*
  store i64* %ln1nh, i64** %Sp_Var
  %ln1ni = bitcast i8* @base_DebugziTrace_traceEvent_info to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1nj = load i64*, i64** %Sp_Var
  %ln1nk = load i64*, i64** %Hp_Var
  %ln1nl = load i64, i64* %R1_Var
  %ln1nm = load i64, i64* %R2_Var
  %ln1nn = load i64, i64* %R3_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1ni( i64* %Base_Arg, i64* %ln1nj, i64* %ln1nk, i64 %ln1nl, i64 %ln1nm, i64 %ln1nn, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
c1fa:
  %ln1no = getelementptr inbounds i64, i64* %Base_Arg, i32 113
  store i64 96, i64* %ln1no, !tbaa !4
  %ln1np = load i64, i64* %R1_Var
  store i64 %ln1np, i64* %R1_Var
  %ln1nq = bitcast i8* @stg_gc_unpt_r1 to void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64)*
  %ln1nr = load i64*, i64** %Sp_Var
  %ln1ns = load i64*, i64** %Hp_Var
  %ln1nt = load i64, i64* %R1_Var
  tail call ghccc void (i64*, i64*, i64*, i64, i64, i64, i64, i64, i64, i64) %ln1nq( i64* %Base_Arg, i64* %ln1nr, i64* %ln1ns, i64 %ln1nt, i64 undef, i64 undef, i64 undef, i64 undef, i64 undef, i64 %SpLim_Arg ) nounwind
  ret void
}
%c1nv_str_struct = type <{[5 x i8]}>
%Profile_zdtrModule2_closure_struct = type <{i64, i64}>
@c1nv_str$def = internal constant %c1nv_str_struct<{[5 x i8] [i8 109, i8 97, i8 105, i8 110, i8 0]}>
@c1nv_str = internal alias i8* bitcast (%c1nv_str_struct* @c1nv_str$def to i8*)
@Profile_zdtrModule2_closure$def = internal global %Profile_zdtrModule2_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TrNameS_static_info to i64), i64 ptrtoint (i8* @c1nv_str to i64)}>
@Profile_zdtrModule2_closure = alias i8* bitcast (%Profile_zdtrModule2_closure_struct* @Profile_zdtrModule2_closure$def to i8*)
%c1nx_str_struct = type <{[8 x i8]}>
%Profile_zdtrModule1_closure_struct = type <{i64, i64}>
@c1nx_str$def = internal constant %c1nx_str_struct<{[8 x i8] [i8 80, i8 114, i8 111, i8 102, i8 105, i8 108, i8 101, i8 0]}>
@c1nx_str = internal alias i8* bitcast (%c1nx_str_struct* @c1nx_str$def to i8*)
@Profile_zdtrModule1_closure$def = internal global %Profile_zdtrModule1_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_TrNameS_static_info to i64), i64 ptrtoint (i8* @c1nx_str to i64)}>
@Profile_zdtrModule1_closure = alias i8* bitcast (%Profile_zdtrModule1_closure_struct* @Profile_zdtrModule1_closure$def to i8*)
%Profile_zdtrModule_closure_struct = type <{i64, i64, i64, i64}>
@Profile_zdtrModule_closure$def = internal global %Profile_zdtrModule_closure_struct<{i64 ptrtoint (i8* @ghczmprim_GHCziTypes_Module_static_info to i64), i64 add (i64 ptrtoint (%Profile_zdtrModule2_closure_struct* @Profile_zdtrModule2_closure$def to i64),i64 1), i64 add (i64 ptrtoint (%Profile_zdtrModule1_closure_struct* @Profile_zdtrModule1_closure$def to i64),i64 1), i64 3}>
@Profile_zdtrModule_closure = alias i8* bitcast (%Profile_zdtrModule_closure_struct* @Profile_zdtrModule_closure$def to i8*)
%S1bV_srt_struct = type <{i64, i64, i64, i64, i64, i64, i64}>
@S1bV_srt$def = internal constant %S1bV_srt_struct<{i64 ptrtoint (%Profile_doTraceFile1_closure_struct* @Profile_doTraceFile1_closure$def to i64), i64 ptrtoint (%Profile_doTraceFile2_closure_struct* @Profile_doTraceFile2_closure$def to i64), i64 ptrtoint (%Profile_doTraceFile3_closure_struct* @Profile_doTraceFile3_closure$def to i64), i64 ptrtoint (%Profile_doTraceFile4_closure_struct* @Profile_doTraceFile4_closure$def to i64), i64 ptrtoint (%Profile_doTraceFile_closure_struct* @Profile_doTraceFile_closure$def to i64), i64 ptrtoint (i8* @base_DebugziTrace_traceEvent_closure to i64), i64 ptrtoint (%Profile_progseq_closure_struct* @Profile_progseq_closure$def to i64)}>
@S1bV_srt = internal alias i8* bitcast (%S1bV_srt_struct* @S1bV_srt$def to i8*)
@newCAF = external global i8
@stg_bh_upd_frame_info = external global i8
@ghczmprim_GHCziCString_unpackCStringzh_info = external global i8
@base_GHCziBase_eqString_info = external global i8
@ghczmprim_GHCziTypes_True_closure = external global i8
@stg_upd_frame_info = external global i8
@ghczmprim_GHCziTypes_ZMZN_closure = external global i8
@base_GHCziShow_zdwshowSignedInt_info = external global i8
@ghczmprim_GHCziTypes_ZC_con_info = external global i8
@stg_gc_pp = external global i8
@ghczmprim_GHCziCString_unpackAppendCStringzh_info = external global i8
@base_GHCziBase_zpzp_info = external global i8
@base_DebugziTrace_traceEvent_info = external global i8
@stg_ap_p_fast = external global i8
@stg_gc_unpt_r1 = external global i8
@ghczmprim_GHCziTypes_TrNameS_static_info = external global i8
@ghczmprim_GHCziTypes_Module_static_info = external global i8
@base_DebugziTrace_traceEvent_closure = external global i8
@llvm.used = appending constant [20 x i8*] [i8* bitcast (%S1bV_srt_struct* @S1bV_srt$def to i8*), i8* bitcast (%Profile_zdtrModule_closure_struct* @Profile_zdtrModule_closure$def to i8*), i8* bitcast (%Profile_zdtrModule1_closure_struct* @Profile_zdtrModule1_closure$def to i8*), i8* bitcast (%c1nx_str_struct* @c1nx_str$def to i8*), i8* bitcast (%Profile_zdtrModule2_closure_struct* @Profile_zdtrModule2_closure$def to i8*), i8* bitcast (%c1nv_str_struct* @c1nv_str$def to i8*), i8* bitcast (%c1eV_str_struct* @c1eV_str$def to i8*), i8* bitcast (%c1eD_str_struct* @c1eD_str$def to i8*), i8* bitcast (%c1eg_str_struct* @c1eg_str$def to i8*), i8* bitcast (%Profile_progseq_closure_struct* @Profile_progseq_closure$def to i8*), i8* bitcast (%Profile_doTraceFile_closure_struct* @Profile_doTraceFile_closure$def to i8*), i8* bitcast (%c1ab_str_struct* @c1ab_str$def to i8*), i8* bitcast (%Profile_doTraceFile4_closure_struct* @Profile_doTraceFile4_closure$def to i8*), i8* bitcast (%c19k_str_struct* @c19k_str$def to i8*), i8* bitcast (%Profile_doTraceFile3_closure_struct* @Profile_doTraceFile3_closure$def to i8*), i8* bitcast (%c18t_str_struct* @c18t_str$def to i8*), i8* bitcast (%Profile_doTraceFile2_closure_struct* @Profile_doTraceFile2_closure$def to i8*), i8* bitcast (%c17C_str_struct* @c17C_str$def to i8*), i8* bitcast (%Profile_doTraceFile1_closure_struct* @Profile_doTraceFile1_closure$def to i8*), i8* bitcast (%__stginit_Profile_struct* @__stginit_Profile$def to i8*)], section "llvm.metadata"
