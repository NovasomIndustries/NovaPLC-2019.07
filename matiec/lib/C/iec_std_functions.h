/*
 * copyright 2008 Edouard TISSERANT
 * copyright 2011 Mario de Sousa (msousa@fe.up.pt)
 *
 * Offered to the public under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.
 *
 * This code is made available on the understanding that it will not be
 * used in safety-critical situations without a full and competent review.
 */

/****
 * IEC 61131-3 standard function library
 */

/* NOTE: This file is full of (what may seem at first) very strange macros.
 *       If you want to know what all these strange macros are doing,
 *       just parse this file through a C preprocessor (e.g. cpp), 
 *       and analyse the output!
 *       $gcc -E iec_std_lib.h 
 */

#ifndef _IEC_STD_FUNCTIONS_H
#define _IEC_STD_FUNCTIONS_H


/*****************************************************************/
/*****************************************************************/
/*****                                                       *****/
/*****                 IEC 61131-3                           *****/
/*****      S T A N D A R D     F U N C T I O N S            *****/
/*****                                                       *****/
/*****************************************************************/
/*****************************************************************/

/* NOTE: If you want to know what all these strange macros are doing,
 *       just parse this file through a C preprocessor, and analyse the output!
 *       $gcc -E iec_std_lib.h 
 */

/* NOTE: We only define and declare the explicitly typed standard functions
 *       (e.g., SIN_REAL, SIN_LREAL, ..., ADD_SINT, ADD_INT, ADD_LINT, ...)
 *       We do not declare/define the overloaded functions
 *       (SIN, ADD, ...). 
 *       When handling a call to an overloaded function, the iec2c compiler 
 *       will determine in stage3 the data type of the parameter being passed, 
 *       and in stage4 generate the C code to call the correct
 *       typed standard function.
 */

/* NOTE on explicit typing of:
 *           - Table 25 - Standard bit shift functions
 *           - Table 29 - Character string Functions
 *
 *  In section 2.5.1.4 (Typing, overloading, and type conversion) of the IEC 61131-3 (version 2)
 *  of the standard, it is stated:
 * "A standard function, [...] is said to be overloaded when it
 * can operate on input data elements of various types within a generic type designator as defined in
 * 2.3.2. For instance, an overloaded addition function on generic type ANY_NUM can operate on data
 * of types LREAL, REAL, DINT, INT, and SINT."
 * [...]
 * "When a function which normally represents an overloaded operator is to be typed, i.e., the types
 * of its inputs and outputs restricted to a particular elementary or derived data type as defined in
 * 2.3, this shall be done by appending an "underline" character followed by the required type, as
 * shown in table 21."
 *
 * However, this explanation (as well as the example in table 21) only refers to functions where the same
 * generic data type is used for the single input and the output parameter.
 * How can we create explicitly types functions when this is not the case?
 * It does not seem to be covered by the standard.
 *
 * For this reason, we do not define the LEN_SINT, LEN_INT, LEN_STRING, LEN_[ANY_INT], LEN_[ANY_STRING] functions...
 */
 

/********************/
/*   EN/ENO PARAMS  */
/********************/
#ifdef DISABLE_EN_ENO_PARAMETERS

  /* Do _not_ generate the EN and ENO parameters! */
  #define EN_ENO_PARAMS
  #define EN_ENO
  #define EN_PFX _NO_EN

  #define TEST_EN(TYPENAME)
  #define TEST_EN_COND(TYPENAME, COND)

#else
    
  /* _Do_ generate the EN and ENO parameters! */
  #define EN_ENO_PARAMS BOOL EN, BOOL *ENO,
  #define EN_ENO EN, ENO,
  #define EN_PFX _EN

  #define TEST_EN(TYPENAME)\
    if (!EN) {\
      if (ENO != NULL)\
        *ENO = __BOOL_LITERAL(FALSE);\
      return __INIT_##TYPENAME;\
    }\
    else if (ENO != NULL)\
      *ENO = __BOOL_LITERAL(TRUE);

  #define TEST_EN_COND(TYPENAME, COND)\
    if (!EN || (COND)) {\
      if (ENO != NULL)\
        *ENO = __BOOL_LITERAL(FALSE);\
      return __INIT_##TYPENAME;\
    }\
    else if (ENO != NULL)\
      *ENO = __BOOL_LITERAL(TRUE);
    
#endif
  
  
/*****************************************/  
/*****************************************/  
/*  2.5.1.5.1 Type Conversion Functions  */
/*****************************************/  
/*****************************************/  

#define __convert_type(from_TYPENAME,to_TYPENAME, oper) \
static inline to_TYPENAME from_TYPENAME##_TO_##to_TYPENAME(EN_ENO_PARAMS from_TYPENAME op){\
  TEST_EN(to_TYPENAME)\
  return (to_TYPENAME)oper(op);\
}

/******** [ANY_NUM | ANY_NBIT]_TO_BOOL   ************/
#define __convert_num_to_bool(TYPENAME) \
static inline BOOL TYPENAME##_TO_BOOL(EN_ENO_PARAMS TYPENAME op){\
  TEST_EN(BOOL)\
  return op == 0 ? 0 : 1;\
}
__ANY_NUM(__convert_num_to_bool)
__ANY_NBIT(__convert_num_to_bool)

/******** [TIME | ANY_DATE]_TO_BOOL   ************/
#define __convert_time_to_bool(TYPENAME) \
static inline BOOL TYPENAME##_TO_BOOL(EN_ENO_PARAMS TYPENAME op){\
  TEST_EN(BOOL)\
  return op.tv_sec == 0 && op.tv_nsec == 0 ? 0 : 1;\
}
__convert_time_to_bool(TIME)
__ANY_DATE(__convert_time_to_bool)

#define __to_anynum_(from_TYPENAME)   __ANY_NUM_1(__iec_,from_TYPENAME)
#define __to_anyint_(from_TYPENAME)   __ANY_INT_1(__iec_,from_TYPENAME)
#define __to_anybit_(from_TYPENAME)   __ANY_BIT_1(__iec_,from_TYPENAME)
#define __to_anynbit_(from_TYPENAME) __ANY_NBIT_1(__iec_,from_TYPENAME)
#define __to_anysint_(from_TYPENAME) __ANY_SINT_1(__iec_,from_TYPENAME)
#define __to_anyuint_(from_TYPENAME) __ANY_UINT_1(__iec_,from_TYPENAME)
#define __to_anyreal_(from_TYPENAME) __ANY_REAL_1(__iec_,from_TYPENAME)
#define __to_anydate_(from_TYPENAME) __ANY_DATE_1(__iec_,from_TYPENAME)

/******** [ANY_BIT]_TO_[ANY_NUM | ANT_NBIT]   ************/
#define __iec_(to_TYPENAME,from_TYPENAME) __convert_type(from_TYPENAME, to_TYPENAME, __move_##to_TYPENAME)
__ANY_BIT(__to_anynum_)
__ANY_BIT(__to_anynbit_)
#undef __iec_

/******** [ANY_INT]_TO_[ANY_NUM | ANT_NBIT]   ************/
#define __iec_(to_TYPENAME,from_TYPENAME) __convert_type(from_TYPENAME, to_TYPENAME, __move_##to_TYPENAME)
__ANY_INT(__to_anynum_)
__ANY_INT(__to_anynbit_)
#undef __iec_

/******** [ANY_REAL]_TO_[ANY_NBIT]   ************/
#define __iec_(to_TYPENAME,from_TYPENAME) __convert_type(from_TYPENAME, to_TYPENAME, __real_to_bit)
__ANY_REAL(__to_anynbit_)
#undef __iec_

/******** [ANY_REAL]_TO_[ANY_NINT]   ************/
#define __iec_(to_TYPENAME,from_TYPENAME) __convert_type(from_TYPENAME, to_TYPENAME, __real_to_sint)
__ANY_REAL(__to_anysint_)
#undef __iec_
#define __iec_(to_TYPENAME,from_TYPENAME) __convert_type(from_TYPENAME, to_TYPENAME, __real_to_uint)
__ANY_REAL(__to_anyuint_)
#undef __iec_

/******** [ANY_REAL]_TO_[ANY_REAL]   ************/ 
#define __iec_(to_TYPENAME,from_TYPENAME) __convert_type(from_TYPENAME, to_TYPENAME, __move_##to_TYPENAME)
__ANY_REAL(__to_anyreal_)
#undef __iec_

/******** [ANY_BIT | ANY_INT]_TO_[TIME | ANY_DATE]   ************/ 
#define __iec_(from_TYPENAME) __convert_type(from_TYPENAME, TIME, __int_to_time)
__ANY_BIT(__iec_)
__ANY_INT(__iec_)
#undef __iec_
#define __iec_(to_TYPENAME,from_TYPENAME) __convert_type(from_TYPENAME, to_TYPENAME, __int_to_time)
__ANY_BIT(__to_anydate_)
__ANY_INT(__to_anydate_)
#undef __iec_

/******** [ANY_REAL]_TO_[TIME | ANY_DATE]   ************/ 
#define __iec_(from_TYPENAME) __convert_type(from_TYPENAME, TIME, __real_to_time)
__ANY_REAL(__iec_)
#undef __iec_
#define __iec_(to_TYPENAME,from_TYPENAME) __convert_type(from_TYPENAME, to_TYPENAME, __real_to_time)
__ANY_REAL(__to_anydate_)
#undef __iec_

/******** [TIME | ANY_DATE]_TO_[ANY_BIT | ANY_INT]   ************/ 
#define __iec_(to_TYPENAME,from_TYPENAME) __convert_type(from_TYPENAME, to_TYPENAME, __time_to_int)
__to_anyint_(TIME)
__to_anynbit_(TIME)
__ANY_DATE(__to_anyint_)
__ANY_DATE(__to_anynbit_)
#undef __iec_

/******** [TIME | ANY_DATE]_TO_[ANY_REAL]   ************/ 
#define __iec_(to_TYPENAME,from_TYPENAME) __convert_type(from_TYPENAME, to_TYPENAME, __time_to_real)
__to_anyreal_(TIME)
__ANY_DATE(__to_anyreal_)
#undef __iec_


/******** [ANY_DATE]_TO_[ANY_DATE | TIME]   ************/ 
/* Not supported: DT_TO_TIME */
__convert_type(DT, DATE,  __date_and_time_to_date)
static inline DATE DATE_AND_TIME_TO_DATE(EN_ENO_PARAMS DT op){
	return DT_TO_DATE(EN_ENO op);
}
__convert_type(DT, DT,    __move_DT)
__convert_type(DT, TOD,   __date_and_time_to_time_of_day)
static inline DATE DATE_AND_TIME_TO_TIME_OF_DAY(EN_ENO_PARAMS DT op){
	return DT_TO_TOD(EN_ENO op);
}
/* Not supported: DATE_TO_TIME */
__convert_type(DATE, DATE, __move_DATE)
/* Not supported: DATE_TO_DT */
/* Not supported: DATE_TO_TOD */
/* Not supported: TOD_TO_TIME */
/* Not supported: TOD_TO_DATE */
/* Not supported: TOD_TO_DT */
__convert_type(TOD, TOD, __move_TOD)


/******** TIME_TO_[ANY_DATE]   ************/ 
/* Not supported: TIME_TO_DATE */
/* Not supported: TIME_TO_DT */
/* Not supported: TIME_TO_TOD */

/******** TIME_TO_TIME   ************/ 
__convert_type(TIME, TIME, __move_TIME)


/******** [ANY_BIT]_TO_STRING   ************/ 
__convert_type(BOOL, STRING, __bool_to_string)
#define __iec_(from_TYPENAME) __convert_type(from_TYPENAME, STRING, __bit_to_string)
__ANY_NBIT(__iec_)
#undef __iec_

/******** [ANY_INT]_TO_STRING   ************/ 
#define __iec_(from_TYPENAME) __convert_type(from_TYPENAME, STRING, __sint_to_string)
__ANY_SINT(__iec_)
#undef __iec_
#define __iec_(from_TYPENAME) __convert_type(from_TYPENAME, STRING, __uint_to_string)
__ANY_UINT(__iec_)
#undef __iec_

/******** [ANY_REAL]_TO_STRING   ************/ 
#define __iec_(from_TYPENAME) __convert_type(from_TYPENAME, STRING, __real_to_string)
__ANY_REAL(__iec_)
#undef __iec_

/******** [ANY_DATE]_TO_STRING   ************/ 
__convert_type(DATE, STRING, __date_to_string)
__convert_type(DT,   STRING, __dt_to_string)
__convert_type(TOD,  STRING, __tod_to_string)

/******** TIME_TO_STRING   ************/ 
__convert_type(TIME, STRING, __time_to_string)


/******** STRING_TO_[ANY_BIT]   ************/ 
__convert_type(STRING, BOOL, __string_to_bool)
#define __iec_(to_TYPENAME) __convert_type(STRING, to_TYPENAME, __string_to_bit)
__ANY_NBIT(__iec_)
#undef __iec_

/******** STRING_TO_[ANY_INT]   ************/ 
#define __iec_(to_TYPENAME) __convert_type(STRING, to_TYPENAME, __string_to_sint)
__ANY_SINT(__iec_)
#undef __iec_
#define __iec_(to_TYPENAME) __convert_type(STRING, to_TYPENAME, __string_to_uint)
__ANY_UINT(__iec_)
#undef __iec_

/******** STRING_TO_[ANY_REAL]   ************/ 
#define __iec_(to_TYPENAME) __convert_type(STRING, to_TYPENAME, __string_to_real)
__ANY_REAL(__iec_)
#undef __iec_

/******** STRING_TO_[ANY_DATE]   ************/ 
#define __iec_(to_TYPENAME) __convert_type(STRING, to_TYPENAME, __string_to_time)
__ANY_DATE(__iec_)
#undef __iec_
                                   
/******** STRING_TO_TIME   ************/ 
__convert_type(STRING, TIME, __string_to_time)


/********   TRUNC   ************/ 
#define __iec_(to_TYPENAME,from_TYPENAME) \
static inline to_TYPENAME TRUNC__##to_TYPENAME##__##from_TYPENAME(EN_ENO_PARAMS from_TYPENAME op){\
  TEST_EN(to_TYPENAME)\
  return (to_TYPENAME)__move_##to_TYPENAME(op);\
}
__ANY_REAL(__to_anyint_)
#undef __iec_


/********   _TO_BCD   ************/
#define __iec_(to_TYPENAME,from_TYPENAME) \
static inline to_TYPENAME from_TYPENAME##_TO_BCD_##to_TYPENAME(EN_ENO_PARAMS from_TYPENAME op){\
  TEST_EN(to_TYPENAME)\
  return (to_TYPENAME)__uint_to_bcd(op);\
}\
static inline to_TYPENAME from_TYPENAME##_TO_BCD__##to_TYPENAME##__##from_TYPENAME(EN_ENO_PARAMS from_TYPENAME op){\
  return from_TYPENAME##_TO_BCD_##to_TYPENAME(EN_ENO op);\
}
__ANY_UINT(__to_anynbit_)
#undef __iec_


/********   BCD_TO_   ************/
#define __iec_(to_TYPENAME,from_TYPENAME) \
static inline to_TYPENAME from_TYPENAME##_BCD_TO_##to_TYPENAME(EN_ENO_PARAMS from_TYPENAME op){\
  TEST_EN_COND(to_TYPENAME, __test_bcd(op))\
  return (to_TYPENAME)__bcd_to_uint(op);\
}\
static inline to_TYPENAME BCD_TO_##to_TYPENAME##__##to_TYPENAME##__##from_TYPENAME(EN_ENO_PARAMS from_TYPENAME op){\
  return from_TYPENAME##_BCD_TO_##to_TYPENAME(EN_ENO op);\
}
__ANY_NBIT(__to_anyuint_)
#undef __iec_


/***********************************/  
/***********************************/  
/*  2.5.1.5.2 Numerical Functions  */
/***********************************/  
/***********************************/  

/* workaround for va-args limitation on shorter than int params */
#define VA_ARGS_REAL LREAL
#define VA_ARGS_LREAL LREAL
#define VA_ARGS_SINT DINT
#define VA_ARGS_INT DINT
#define VA_ARGS_DINT DINT
#define VA_ARGS_LINT LINT
#define VA_ARGS_USINT UDINT
#define VA_ARGS_UINT UDINT
#define VA_ARGS_UDINT UDINT
#define VA_ARGS_ULINT ULINT
#define VA_ARGS_TIME TIME
#define VA_ARGS_BOOL DWORD
#define VA_ARGS_BYTE DWORD
#define VA_ARGS_WORD DWORD
#define VA_ARGS_DWORD DWORD
#define VA_ARGS_LWORD LWORD
#define VA_ARGS_STRING STRING
#define VA_ARGS_WSTRING WSTRING
#define VA_ARGS_DATE DATE
#define VA_ARGS_TOD TOD
#define VA_ARGS_DT DT


#define __numeric(fname,TYPENAME, FUNC) \
/* explicitly typed function */\
static inline TYPENAME fname##TYPENAME(EN_ENO_PARAMS TYPENAME op){\
  TEST_EN(TYPENAME)\
  return FUNC(op);\
}\
/* overloaded function */\
static inline TYPENAME fname##_##TYPENAME##__##TYPENAME(EN_ENO_PARAMS TYPENAME op) {\
  return fname##TYPENAME(EN_ENO op);\
}

/******************************************************************/
/***   Table 23 - Standard functions of one numeric variable    ***/
/******************************************************************/

  /**************/
  /*    ABS     */
  /**************/
#define __abs_signed(TYPENAME) \
/* explicitly typed function */\
static inline TYPENAME ABS_##TYPENAME(EN_ENO_PARAMS TYPENAME op){\
  TEST_EN(TYPENAME)\
  if (op < 0)\
    return -op;\
  return op;\
}\
/* overloaded function */\
static inline TYPENAME ABS__##TYPENAME##__##TYPENAME(EN_ENO_PARAMS TYPENAME op) {\
  return ABS_##TYPENAME(EN_ENO op);\
}

#define __abs_unsigned(TYPENAME) \
/* explicitly typed function */\
static inline TYPENAME ABS_##TYPENAME(EN_ENO_PARAMS TYPENAME op){\
  TEST_EN(TYPENAME)\
  return op;\
}\
/* overloaded function */\
static inline TYPENAME ABS__##TYPENAME##__##TYPENAME(EN_ENO_PARAMS TYPENAME op) {\
  return ABS_##TYPENAME(EN_ENO op);\
}

__ANY_REAL(__abs_signed)
__ANY_SINT(__abs_signed)
__ANY_UINT(__abs_unsigned)


  /**************/
  /*    SQRT    */
  /**************/
#define __sqrt(TYPENAME) __numeric(SQRT_, TYPENAME, sqrt)
__ANY_REAL(__sqrt)


/**************/
  /*     LN     */
  /**************/
#define __ln(TYPENAME) __numeric(LN_, TYPENAME, log)
__ANY_REAL(__ln)


  /**************/
  /*     LOG    */
  /**************/
#define __log(TYPENAME) __numeric(LOG_, TYPENAME, log10)
__ANY_REAL(__log)


  /**************/
  /*     EXP    */
  /**************/
#define __exp(TYPENAME) __numeric(EXP_, TYPENAME, exp)
__ANY_REAL(__exp)


  /**************/
  /*     SIN    */
  /**************/
#define __sin(TYPENAME) __numeric(SIN_, TYPENAME, sin)
__ANY_REAL(__sin)


  /**************/
  /*     COS    */
  /**************/
#define __cos(TYPENAME) __numeric(COS_, TYPENAME, cos)
__ANY_REAL(__cos)

  /**************/
  /*     TAN    */
  /**************/
#define __tan(TYPENAME) __numeric(TAN_, TYPENAME, tan)
__ANY_REAL(__tan)


  /**************/
  /*    ASIN    */
  /**************/
#define __asin(TYPENAME) __numeric(ASIN_, TYPENAME, asin)
__ANY_REAL(__asin)

  /**************/
  /*    ACOS    */
  /**************/
#define __acos(TYPENAME) __numeric(ACOS_, TYPENAME, acos)
__ANY_REAL(__acos)

  /**************/
  /*    ATAN    */
  /**************/
#define __atan(TYPENAME) __numeric(ATAN_, TYPENAME, atan)
__ANY_REAL(__atan)



/*****************************************************/
/***   Table 24 - Standard arithmetic functions    ***/
/*****************************************************/

/* NOTE: unlike all other standard functions, the arithmetic based (extensible) functions
 *       (ADD, MUL, AND, OR, XOR) have several implementations:
 *        - 8 optimized versions for when the functon is called with 1 to 8 INput parameters
 *        - an extra non-optimized version for every other case. 
 *       
 *       This means that instead of the functions usually named 
 *           ADD_TTTT() 
 *           ADD__TTTT__TTTT()        (where TTTT is a type, e.g. INT)
 *       we have instead
 *           ADD_TTTT1(),  ADD_TTTT2(), ... ,ADD_TTTT8()
 *           ADD_TTTTX()              
 *           ADD_TTTT_VAR()           
 *           ADD__TTTT__TTTT1(), ..., ADD__TTTT__TTTT8()       
 *           ADD__TTTT__TTTT_VAR()    
 *       
 *       The standard names
 *           ADD_TTTT() 
 *           ADD__TTTT__TTTT()
 *       are instead later (in this .h file) declared as macros, which will choose
 *       at compilation time the correct version/implementation to call
 *       based on the number of input parameters in that specific call.
 */
#define __arith_expand(fname, TYPENAME, OP)				\
									\
	static inline TYPENAME fname##1(EN_ENO_PARAMS			\
					TYPENAME op1) {			\
		TEST_EN(TYPENAME);					\
		return op1;						\
	}								\
									\
	static inline TYPENAME fname##2(EN_ENO_PARAMS			\
					TYPENAME op1,			\
					TYPENAME op2) {			\
		TEST_EN(TYPENAME);					\
		op1 = op1 OP op2;					\
		return op1;						\
	}								\
									\
	static inline TYPENAME fname##3(EN_ENO_PARAMS			\
					TYPENAME op1,			\
					TYPENAME op2,			\
					TYPENAME op3){			\
		TEST_EN(TYPENAME);					\
		op1 = op1 OP op2 OP op3;				\
		return op1;						\
	}								\
									\
	static inline TYPENAME fname##4(EN_ENO_PARAMS			\
					TYPENAME op1,			\
					TYPENAME op2,			\
					TYPENAME op3,			\
					TYPENAME op4){			\
		TEST_EN(TYPENAME);					\
		op1 = op1 OP op2 OP op3 OP op4;				\
		return op1;						\
	}								\
									\
	static inline TYPENAME fname##5(EN_ENO_PARAMS			\
					TYPENAME op1,			\
					TYPENAME op2,			\
					TYPENAME op3,			\
					TYPENAME op4,			\
					TYPENAME op5){			\
		TEST_EN(TYPENAME);					\
		op1 = op1 OP op2 OP op3 OP op4 OP op5;			\
		return op1;						\
	}								\
									\
	static inline TYPENAME fname##6(EN_ENO_PARAMS			\
					TYPENAME op1,			\
					TYPENAME op2,			\
					TYPENAME op3,			\
					TYPENAME op4,			\
					TYPENAME op5,			\
					TYPENAME op6){			\
		TEST_EN(TYPENAME);					\
		op1 = op1 OP op2 OP op3 OP op4 OP op5 OP op6;		\
		return op1;						\
	}								\
									\
	static inline TYPENAME fname##7(EN_ENO_PARAMS			\
					TYPENAME op1,			\
					TYPENAME op2,			\
					TYPENAME op3,			\
					TYPENAME op4,			\
					TYPENAME op5,			\
					TYPENAME op6,			\
					TYPENAME op7){			\
		TEST_EN(TYPENAME);					\
		op1 = op1 OP op2 OP op3 OP op4 OP op5 OP op6 OP op7;	\
		return op1;						\
	}								\
									\
	static inline TYPENAME fname##8(EN_ENO_PARAMS			\
					TYPENAME op1,			\
					TYPENAME op2,			\
					TYPENAME op3,			\
					TYPENAME op4,			\
					TYPENAME op5,			\
					TYPENAME op6,			\
					TYPENAME op7,			\
					TYPENAME op8){			\
		TEST_EN(TYPENAME);					\
		op1 =   op1 OP op2 OP op3 OP op4 OP			\
			op5 OP op6 OP op7 OP op8;			\
		return op1;						\
	}								\
									\
	static inline TYPENAME fname##_VAR(EN_ENO_PARAMS		\
					UINT param_count,		\
					TYPENAME op1, ...){		\
		va_list ap;						\
		UINT i;							\
		TEST_EN(TYPENAME)					\
									\
			va_start (ap, op1);         /* Initialize the argument list.  */ \
									\
		for (i = 0; i < param_count - 1; i++){			\
			op1 = op1 OP va_arg (ap, VA_ARGS_##TYPENAME);	\
		}							\
									\
		va_end (ap);                  /* Clean up.  */		\
		return op1;						\
	}								\
									\

/* macros needed for #if-like behaviour inside of macro */
#define ARITH_OPERATION_COND_IGNORE(...)
#define ARITH_OPERATION_COND_IDENT(...) __VA_ARGS__
#define ARITH_OPERATION_COND_SKIP_
#define ARITH_OPERATION_COND_CLAUSE1(...) __VA_ARGS__ ARITH_OPERATION_COND_IGNORE
#define ARITH_OPERATION_COND_CLAUSE2(...) ARITH_OPERATION_COND_IDENT

#define ARITH_OPERATION_COND_(A,B,C,...) C
#define ARITH_OPERATION_COND(EXP)			\
	ARITH_OPERATION_COND_(				\
		EXP,					\
		ARITH_OPERATION_COND_CLAUSE1,		\
		ARITH_OPERATION_COND_CLAUSE2, ~)

#define ARITH_OPERATION_LE_(X, Y) ARITH_OPERATION_COND_ ## X ## _LE_ ## Y
#define ARITH_OPERATION_LE( X, Y) ARITH_OPERATION_LE_(X, Y)

/*
 * Macro to remove typecast in argument number placed by code generator:
 * (UINT)3 -> 3
 */
#define CLEAR_TYPECAST(X)
#define MAX_INLINE_PARAM_COUNT 8
#ifdef DISABLE_EN_ENO_PARAMETERS

#define ARITH_OPERATION_CALL__(FNAME, PARAM_COUNT, ...)			\
	ARITH_OPERATION_COND(ARITH_OPERATION_LE(PARAM_COUNT, MAX_INLINE_PARAM_COUNT)) \
	(FNAME##_VAR(PARAM_COUNT, __VA_ARGS__))				\
	(FNAME##PARAM_COUNT(      __VA_ARGS__))

#define ARITH_OPERATION_CALL_( FNAME, PARAM_COUNT, ...)    ARITH_OPERATION_CALL__(FNAME, PARAM_COUNT, __VA_ARGS__)
#define ARITH_OPERATION_CALL(  FNAME, PARAM_COUNT, ...)    ARITH_OPERATION_CALL_( FNAME, CLEAR_TYPECAST PARAM_COUNT, __VA_ARGS__)

#else

#define ARITH_OPERATION_CALL__(EN, ENO, FNAME, PARAM_COUNT, ...)	\
	ARITH_OPERATION_COND(ARITH_OPERATION_LE(PARAM_COUNT, MAX_INLINE_PARAM_COUNT)) \
	(FNAME##_VAR(       (EN), (ENO), PARAM_COUNT, __VA_ARGS__))	\
	(FNAME##PARAM_COUNT((EN), (ENO), __VA_ARGS__))


#define ARITH_OPERATION_CALL_( EN, ENO, FNAME, PARAM_COUNT, ...)    ARITH_OPERATION_CALL__(EN, ENO, FNAME, PARAM_COUNT, __VA_ARGS__)
#define ARITH_OPERATION_CALL(  EN, ENO, FNAME, PARAM_COUNT, ...)    ARITH_OPERATION_CALL_(EN, ENO, FNAME, CLEAR_TYPECAST PARAM_COUNT, __VA_ARGS__)

#endif

#ifdef DISABLE_EN_ENO_PARAMETERS
#define ARITH_OPERATION__TYPE__TYPE_NO_EN(FNAME, PARAM_COUNT, ...)             ARITH_OPERATION_CALL(FNAME, PARAM_COUNT, __VA_ARGS__)
#else
#define ARITH_OPERATION__TYPE__TYPE_EN(FNAME, EN, ENO, PARAM_COUNT, ...)       ARITH_OPERATION_CALL(EN, ENO, FNAME, PARAM_COUNT, __VA_ARGS__)
#endif

#define ARITH_OPERATION__TYPE__TYPE_(PFX, ...)               ARITH_OPERATION__TYPE__TYPE##PFX(__VA_ARGS__)
#define ARITH_OPERATION__TYPE__TYPE( PFX, ...)               ARITH_OPERATION__TYPE__TYPE_(PFX, __VA_ARGS__)



#define __arith_static(fname,TYPENAME, OP)\
/* explicitly typed function */\
static inline TYPENAME fname##TYPENAME(EN_ENO_PARAMS TYPENAME op1, TYPENAME op2){\
  TEST_EN(TYPENAME)\
  return op1 OP op2;\
}\
/* overloaded function */\
static inline TYPENAME fname##_##TYPENAME##__##TYPENAME##__##TYPENAME(EN_ENO_PARAMS TYPENAME op1, TYPENAME op2){\
  return fname##TYPENAME(EN_ENO op1, op2);\
}

  /**************/
  /*     ADD    */
  /**************/
#define __add(TYPENAME) \
__arith_expand(ADD_##TYPENAME, TYPENAME, +) 			 /* explicitly typed function */\
__arith_expand(ADD__##TYPENAME##__##TYPENAME, TYPENAME, +)	 /* overloaded function */
__ANY_NUM(__add)

#define ADD_SINT(...)          ARITH_OPERATION__TYPE__TYPE(EN_PFX, ADD_SINT,            __VA_ARGS__)
#define ADD__SINT__SINT(...)   ARITH_OPERATION__TYPE__TYPE(EN_PFX, ADD__SINT__SINT,     __VA_ARGS__)

#define ADD_INT(...)           ARITH_OPERATION__TYPE__TYPE(EN_PFX, ADD_INT,             __VA_ARGS__)
#define ADD__INT__INT(...)     ARITH_OPERATION__TYPE__TYPE(EN_PFX, ADD__INT__INT,       __VA_ARGS__)

#define ADD_DINT(...)          ARITH_OPERATION__TYPE__TYPE(EN_PFX, ADD_DINT,            __VA_ARGS__)
#define ADD__DINT__DINT(...)   ARITH_OPERATION__TYPE__TYPE(EN_PFX, ADD__DINT__DINT,     __VA_ARGS__)

#define ADD_LINT(...)          ARITH_OPERATION__TYPE__TYPE(EN_PFX, ADD_LINT,            __VA_ARGS__)
#define ADD__LINT__LINT(...)   ARITH_OPERATION__TYPE__TYPE(EN_PFX, ADD__LINT__LINT,     __VA_ARGS__)

#define ADD_USINT(...)         ARITH_OPERATION__TYPE__TYPE(EN_PFX, ADD_USINT,           __VA_ARGS__)
#define ADD__USINT__USINT(...) ARITH_OPERATION__TYPE__TYPE(EN_PFX, ADD__USINT__USINT,   __VA_ARGS__)

#define ADD_UINT(...)          ARITH_OPERATION__TYPE__TYPE(EN_PFX, ADD_UINT,            __VA_ARGS__)
#define ADD__UINT__UINT(...)   ARITH_OPERATION__TYPE__TYPE(EN_PFX, ADD__UINT__UINT,     __VA_ARGS__)

#define ADD_UDINT(...)         ARITH_OPERATION__TYPE__TYPE(EN_PFX, ADD_UDINT,           __VA_ARGS__)
#define ADD__UDINT__UDINT(...) ARITH_OPERATION__TYPE__TYPE(EN_PFX, ADD__UDINT__UDINT,   __VA_ARGS__)

#define ADD_ULINT(...)         ARITH_OPERATION__TYPE__TYPE(EN_PFX, ADD_ULINT,           __VA_ARGS__)
#define ADD__ULINT__ULINT(...) ARITH_OPERATION__TYPE__TYPE(EN_PFX, ADD__ULINT__ULINT,   __VA_ARGS__)

#define ADD_REAL(...)          ARITH_OPERATION__TYPE__TYPE(EN_PFX, ADD_REAL,            __VA_ARGS__)
#define ADD__REAL__REAL(...)   ARITH_OPERATION__TYPE__TYPE(EN_PFX, ADD__REAL__REAL,     __VA_ARGS__)

#define ADD_LREAL(...)         ARITH_OPERATION__TYPE__TYPE(EN_PFX, ADD_LREAL,           __VA_ARGS__)
#define ADD__LREAL__LREAL(...) ARITH_OPERATION__TYPE__TYPE(EN_PFX, ADD__LREAL__LREAL,   __VA_ARGS__)


  /**************/
  /*     MUL    */
  /**************/
#define __mul(TYPENAME) \
__arith_expand(MUL_##TYPENAME, TYPENAME, *) 			 /* explicitly typed function */\
__arith_expand(MUL__##TYPENAME##__##TYPENAME, TYPENAME, *)	 /* overloaded function */
__ANY_NUM(__mul)

#define MUL_SINT(...)          ARITH_OPERATION__TYPE__TYPE(EN_PFX, MUL_SINT,            __VA_ARGS__)
#define MUL__SINT__SINT(...)   ARITH_OPERATION__TYPE__TYPE(EN_PFX, MUL__SINT__SINT,     __VA_ARGS__)

#define MUL_INT(...)           ARITH_OPERATION__TYPE__TYPE(EN_PFX, MUL_INT,             __VA_ARGS__)
#define MUL__INT__INT(...)     ARITH_OPERATION__TYPE__TYPE(EN_PFX, MUL__INT__INT,       __VA_ARGS__)

#define MUL_DINT(...)          ARITH_OPERATION__TYPE__TYPE(EN_PFX, MUL_DINT,            __VA_ARGS__)
#define MUL__DINT__DINT(...)   ARITH_OPERATION__TYPE__TYPE(EN_PFX, MUL__DINT__DINT,     __VA_ARGS__)

#define MUL_LINT(...)          ARITH_OPERATION__TYPE__TYPE(EN_PFX, MUL_LINT,            __VA_ARGS__)
#define MUL__LINT__LINT(...)   ARITH_OPERATION__TYPE__TYPE(EN_PFX, MUL__LINT__LINT,     __VA_ARGS__)

#define MUL_USINT(...)         ARITH_OPERATION__TYPE__TYPE(EN_PFX, MUL_USINT,           __VA_ARGS__)
#define MUL__USINT__USINT(...) ARITH_OPERATION__TYPE__TYPE(EN_PFX, MUL__USINT__USINT,   __VA_ARGS__)

#define MUL_UINT(...)          ARITH_OPERATION__TYPE__TYPE(EN_PFX, MUL_UINT,            __VA_ARGS__)
#define MUL__UINT__UINT(...)   ARITH_OPERATION__TYPE__TYPE(EN_PFX, MUL__UINT__UINT,     __VA_ARGS__)

#define MUL_UDINT(...)         ARITH_OPERATION__TYPE__TYPE(EN_PFX, MUL_UDINT,           __VA_ARGS__)
#define MUL__UDINT__UDINT(...) ARITH_OPERATION__TYPE__TYPE(EN_PFX, MUL__UDINT__UDINT,   __VA_ARGS__)

#define MUL_ULINT(...)         ARITH_OPERATION__TYPE__TYPE(EN_PFX, MUL_ULINT,           __VA_ARGS__)
#define MUL__ULINT__ULINT(...) ARITH_OPERATION__TYPE__TYPE(EN_PFX, MUL__ULINT__ULINT,   __VA_ARGS__)

#define MUL_REAL(...)          ARITH_OPERATION__TYPE__TYPE(EN_PFX, MUL_REAL,            __VA_ARGS__)
#define MUL__REAL__REAL(...)   ARITH_OPERATION__TYPE__TYPE(EN_PFX, MUL__REAL__REAL,     __VA_ARGS__)

#define MUL_LREAL(...)         ARITH_OPERATION__TYPE__TYPE(EN_PFX, MUL_LREAL,           __VA_ARGS__)
#define MUL__LREAL__LREAL(...) ARITH_OPERATION__TYPE__TYPE(EN_PFX, MUL__LREAL__LREAL,   __VA_ARGS__)


  /**************/
  /*     SUB    */
  /**************/
#define __sub(TYPENAME) __arith_static(SUB_, TYPENAME, -)
__ANY_NUM(__sub)


  /**************/
  /*     DIV    */
  /**************/
#define __div(TYPENAME)\
/* The explicitly typed standard functions */\
static inline TYPENAME DIV_##TYPENAME(EN_ENO_PARAMS TYPENAME op1, TYPENAME op2){\
  TEST_EN_COND(TYPENAME, op2 == 0)\
  return op1 / op2;\
}\
/* The overloaded standard functions */\
static inline TYPENAME DIV__##TYPENAME##__##TYPENAME##__##TYPENAME(EN_ENO_PARAMS TYPENAME op1, TYPENAME op2){\
  return DIV_##TYPENAME(EN_ENO op1, op2);\
}
__ANY_NUM(__div)


  /**************/
  /*     MOD    */
  /**************/
#define __mod(TYPENAME)\
/* The explicitly typed standard functions */\
static inline TYPENAME MOD_##TYPENAME(EN_ENO_PARAMS TYPENAME op1, TYPENAME op2){\
  TEST_EN(TYPENAME)\
  if (op2 == 0) return 0;\
  return op1 % op2;\
}\
/* The overloaded standard functions */\
static inline TYPENAME MOD__##TYPENAME##__##TYPENAME##__##TYPENAME(EN_ENO_PARAMS TYPENAME op1, TYPENAME op2){\
  return MOD_##TYPENAME(EN_ENO op1, op2);\
}
__ANY_INT(__mod)

  /**************/
  /*    EXPT    */
  /**************/
/* overloaded function */
#define __iec_(in1_TYPENAME,in2_TYPENAME) \
static inline in1_TYPENAME EXPT__##in1_TYPENAME##__##in1_TYPENAME##__##in2_TYPENAME\
  (EN_ENO_PARAMS in1_TYPENAME IN1, in2_TYPENAME IN2){\
  TEST_EN(in1_TYPENAME)\
  return __expt(IN1, IN2);\
}
#define __in1_anyreal_(in2_TYPENAME)   __ANY_REAL_1(__iec_,in2_TYPENAME)
__ANY_NUM(__in1_anyreal_)
#undef __iec_

  

  /***************/
  /*     MOVE    */
  /***************/
/* The explicitly typed standard functions */
#define __iec_(TYPENAME)\
static inline TYPENAME MOVE_##TYPENAME(EN_ENO_PARAMS TYPENAME op1){\
  TEST_EN(TYPENAME)\
  return op1;\
}
__ANY(__iec_)
#undef __iec_

/* Overloaded function */
#define __iec_(TYPENAME)\
static inline TYPENAME MOVE__##TYPENAME##__##TYPENAME(EN_ENO_PARAMS TYPENAME op1){\
  TEST_EN(TYPENAME)\
  return op1;\
}
__ANY(__iec_)
#undef __iec_






/***********************************/  
/***********************************/  
/*  2.5.1.5.3 Bit String Functions */
/***********************************/  
/***********************************/  

/****************************************************/
/***   Table 25 - Standard bit shift functions    ***/
/****************************************************/

/* We do not delcare explcitly typed versions of the functions in table 25.
 * See note above regarding explicitly typed functions for more details.
 */
#define __in1_anynbit_(in2_TYPENAME)   __ANY_NBIT_1(__iec_,in2_TYPENAME)

#define __shift_(fname, in1_TYPENAME, in2_TYPENAME, OP)\
static inline in1_TYPENAME fname(EN_ENO_PARAMS in1_TYPENAME IN, in2_TYPENAME N) {\
  TEST_EN(in1_TYPENAME)\
  return IN OP N;\
}

  /**************/
  /*     SHL    */
  /**************/
#define __iec_(TYPENAME) \
/* Overloaded function */\
static inline BOOL SHL__BOOL__##TYPENAME(EN_ENO_PARAMS BOOL IN, TYPENAME N) { \
  TEST_EN(BOOL);\
  return (N==0)? IN : __INIT_BOOL;  /* shifting by N>1 will always introduce a 0 */\
}
__ANY_INT(__iec_)
#undef __iec_


#define __iec_(in1_TYPENAME,in2_TYPENAME) \
__shift_(SHL__##in1_TYPENAME##__##in1_TYPENAME##__##in2_TYPENAME, in1_TYPENAME, in2_TYPENAME, << )/* Overloaded function */
__ANY_INT(__in1_anynbit_)
#undef __iec_


  /**************/
  /*     SHR    */
  /**************/
#define __iec_(TYPENAME) \
/* Overloaded function */\
static inline BOOL SHR__BOOL__##TYPENAME(EN_ENO_PARAMS BOOL IN, TYPENAME N) { \
  TEST_EN(BOOL);\
  return (N==0)? IN : __INIT_BOOL;  /* shifting by N>1 will always introduce a 0 */\
}
__ANY_INT(__iec_)
#undef __iec_


#define __iec_(in1_TYPENAME,in2_TYPENAME) \
__shift_(SHR__##in1_TYPENAME##__##in1_TYPENAME##__##in2_TYPENAME, in1_TYPENAME, in2_TYPENAME, >> )/* Overloaded function */
__ANY_INT(__in1_anynbit_)
#undef __iec_


  /**************/
  /*     ROR    */
  /**************/
#define __iec_(TYPENAME) \
/* Overloaded function */\
static inline BOOL ROR__BOOL__##TYPENAME(EN_ENO_PARAMS BOOL IN, TYPENAME N) { \
  TEST_EN(BOOL);\
  return IN; /* rotating a single bit by any value N will not change that bit! */\
}
__ANY_INT(__iec_)
#undef __iec_


#define __iec_(in1_TYPENAME,in2_TYPENAME) \
static inline in1_TYPENAME ROR__##in1_TYPENAME##__##in1_TYPENAME##__##in2_TYPENAME(EN_ENO_PARAMS in1_TYPENAME IN, in2_TYPENAME N){\
  TEST_EN(in1_TYPENAME)\
  N %= 8*sizeof(in1_TYPENAME);\
  return (IN >> N) | (IN << (8*sizeof(in1_TYPENAME)-N));\
}
__ANY_INT(__in1_anynbit_)
#undef __iec_


  /**************/
  /*     ROL    */
  /**************/
#define __iec_(TYPENAME) \
/* Overloaded function */\
static inline BOOL ROL__BOOL__##TYPENAME(EN_ENO_PARAMS BOOL IN, TYPENAME N) { \
  TEST_EN(BOOL);\
  return IN; /* rotating a single bit by any value N will not change that bit! */\
}
__ANY_INT(__iec_)
#undef __iec_


#define __iec_(in1_TYPENAME,in2_TYPENAME) \
static inline in1_TYPENAME ROL__##in1_TYPENAME##__##in1_TYPENAME##__##in2_TYPENAME(EN_ENO_PARAMS in1_TYPENAME IN, in2_TYPENAME N){\
  TEST_EN(in1_TYPENAME)\
  N %= 8*sizeof(in1_TYPENAME);\
  return (IN << N) | (IN >> (8*sizeof(in1_TYPENAME)-N));\
}
__ANY_INT(__in1_anynbit_)
#undef __iec_



/*********************/
/***   Table 26    ***/
/*********************/

  /**************/
  /*     AND    */
  /**************/
__arith_expand(AND_BOOL, BOOL, && )         /* The explicitly typed standard functions */
__arith_expand(AND__BOOL__BOOL, BOOL, && )  /* Overloaded function */
#define AND_BOOL(...)        ARITH_OPERATION__TYPE__TYPE(EN_PFX, AND_BOOL,        __VA_ARGS__)
#define AND__BOOL__BOOL(...) ARITH_OPERATION__TYPE__TYPE(EN_PFX, AND__BOOL__BOOL, __VA_ARGS__)

#define __iec_(TYPENAME) \
__arith_expand(AND_##TYPENAME, TYPENAME, &)  /* The explicitly typed standard functions */\
__arith_expand(AND__##TYPENAME##__##TYPENAME, TYPENAME, &)  /* Overloaded function */
__ANY_NBIT(__iec_)
#undef __iec_

#define AND_BYTE(...)          ARITH_OPERATION__TYPE__TYPE(EN_PFX, AND_BYTE,             __VA_ARGS__)
#define AND__BYTE__BYTE(...)   ARITH_OPERATION__TYPE__TYPE(EN_PFX, AND__BYTE__BYTE,      __VA_ARGS__)

#define AND_WORD(...)          ARITH_OPERATION__TYPE__TYPE(EN_PFX, AND_WORD,             __VA_ARGS__)
#define AND__WORD__WORD(...)   ARITH_OPERATION__TYPE__TYPE(EN_PFX, AND__WORD__WORD,      __VA_ARGS__)

#define AND_DWORD(...)         ARITH_OPERATION__TYPE__TYPE(EN_PFX, AND_DWORD,            __VA_ARGS__)
#define AND__DWORD__DWORD(...) ARITH_OPERATION__TYPE__TYPE(EN_PFX, AND__DWORD__DWORD,    __VA_ARGS__)

#define AND_LWORD(...)         ARITH_OPERATION__TYPE__TYPE(EN_PFX, AND_LWORD,            __VA_ARGS__)
#define AND__LWORD__LWORD(...) ARITH_OPERATION__TYPE__TYPE(EN_PFX, AND__LWORD__LWORD,    __VA_ARGS__)


  /*************/
  /*     OR    */
  /*************/
__arith_expand(OR_BOOL, BOOL, || )         /* The explicitly typed standard functions */
__arith_expand(OR__BOOL__BOOL, BOOL, || )  /* Overloaded function */
#define OR_BOOL(...)        ARITH_OPERATION__TYPE__TYPE(EN_PFX, OR_BOOL,        __VA_ARGS__)
#define OR__BOOL__BOOL(...) ARITH_OPERATION__TYPE__TYPE(EN_PFX, OR__BOOL__BOOL, __VA_ARGS__)

#define __iec_(TYPENAME) \
__arith_expand(OR_##TYPENAME, TYPENAME, |)  /* The explicitly typed standard functions */\
__arith_expand(OR__##TYPENAME##__##TYPENAME, TYPENAME, |)  /* Overloaded function */
__ANY_NBIT(__iec_)
#undef __iec_

#define OR_BYTE(...)          ARITH_OPERATION__TYPE__TYPE(EN_PFX, OR_BYTE,             __VA_ARGS__)
#define OR__BYTE__BYTE(...)   ARITH_OPERATION__TYPE__TYPE(EN_PFX, OR__BYTE__BYTE,      __VA_ARGS__)

#define OR_WORD(...)          ARITH_OPERATION__TYPE__TYPE(EN_PFX, OR_WORD,             __VA_ARGS__)
#define OR__WORD__WORD(...)   ARITH_OPERATION__TYPE__TYPE(EN_PFX, OR__WORD__WORD,      __VA_ARGS__)

#define OR_DWORD(...)         ARITH_OPERATION__TYPE__TYPE(EN_PFX, OR_DWORD,            __VA_ARGS__)
#define OR__DWORD__DWORD(...) ARITH_OPERATION__TYPE__TYPE(EN_PFX, OR__DWORD__DWORD,    __VA_ARGS__)

#define OR_LWORD(...)         ARITH_OPERATION__TYPE__TYPE(EN_PFX, OR_LWORD,            __VA_ARGS__)
#define OR__LWORD__LWORD(...) ARITH_OPERATION__TYPE__TYPE(EN_PFX, OR__LWORD__LWORD,    __VA_ARGS__)


  /**************/
  /*     XOR    */
  /**************/
#define __xorbool_expand(fname) \
static inline BOOL fname(EN_ENO_PARAMS UINT param_count, BOOL op1, ...){ \
  va_list ap; \
  UINT i; \
  TEST_EN(BOOL) \
\
  va_start (ap, op1);         /* Initialize the argument list.  */ \
\
  for (i = 0; i < param_count - 1; i++){ \
    BOOL tmp = va_arg (ap, VA_ARGS_BOOL); \
    op1 = (op1 && !tmp) || (!op1 && tmp); \
  } \
\
  va_end (ap);                  /* Clean up.  */ \
  return op1; \
}

__xorbool_expand(XOR_BOOL) /* The explicitly typed standard functions */
__xorbool_expand(XOR__BOOL__BOOL) /* Overloaded function */

#define __iec_(TYPENAME) \
__arith_expand(XOR_##TYPENAME, TYPENAME, ^) /* The explicitly typed standard functions */\
__arith_expand(XOR__##TYPENAME##__##TYPENAME, TYPENAME, ^) /* Overloaded function */
__ANY_NBIT(__iec_)
#undef __iec_

#define XOR_BYTE(...)          ARITH_OPERATION__TYPE__TYPE(EN_PFX, XOR_BYTE,             __VA_ARGS__)
#define XOR__BYTE__BYTE(...)   ARITH_OPERATION__TYPE__TYPE(EN_PFX, XOR__BYTE__BYTE,      __VA_ARGS__)

#define XOR_WORD(...)          ARITH_OPERATION__TYPE__TYPE(EN_PFX, XOR_WORD,             __VA_ARGS__)
#define XOR__WORD__WORD(...)   ARITH_OPERATION__TYPE__TYPE(EN_PFX, XOR__WORD__WORD,      __VA_ARGS__)

#define XOR_DWORD(...)         ARITH_OPERATION__TYPE__TYPE(EN_PFX, XOR_DWORD,            __VA_ARGS__)
#define XOR__DWORD__DWORD(...) ARITH_OPERATION__TYPE__TYPE(EN_PFX, XOR__DWORD__DWORD,    __VA_ARGS__)

#define XOR_LWORD(...)         ARITH_OPERATION__TYPE__TYPE(EN_PFX, XOR_LWORD,            __VA_ARGS__)
#define XOR__LWORD__LWORD(...) ARITH_OPERATION__TYPE__TYPE(EN_PFX, XOR__LWORD__LWORD,    __VA_ARGS__)


  /**************/
  /*     NOT    */
  /**************/
/* The explicitly typed standard functions */
static inline BOOL NOT_BOOL(EN_ENO_PARAMS BOOL op1){
  TEST_EN(BOOL)
  return !op1;
}

/* Overloaded function */
static inline BOOL NOT__BOOL__BOOL(EN_ENO_PARAMS BOOL op1){
  TEST_EN(BOOL)
  return !op1;
}

/* The explicitly typed standard functions */
#define __iec_(TYPENAME)\
static inline TYPENAME NOT_##TYPENAME(EN_ENO_PARAMS TYPENAME op1){\
  TEST_EN(TYPENAME)\
  return ~op1;\
}
__ANY_NBIT(__iec_)
#undef __iec_

/* Overloaded function */
#define __iec_(TYPENAME)\
static inline TYPENAME NOT__##TYPENAME##__##TYPENAME(EN_ENO_PARAMS TYPENAME op1){\
  TEST_EN(TYPENAME)\
  return ~op1;\
}
__ANY_NBIT(__iec_)
#undef __iec_






/***************************************************/  
/***************************************************/  
/*  2.5.1.5.4  Selection and comparison Functions  */
/***************************************************/  
/***************************************************/  

/*********************/
/***   Table 27    ***/
/*********************/


    /**************/
    /*    SEL     */
    /**************/

/* The explicitly typed standard functions */
#define __iec_(TYPENAME)\
static inline TYPENAME SEL_##TYPENAME(EN_ENO_PARAMS BOOL G, TYPENAME op0, TYPENAME op1){\
  TEST_EN(TYPENAME)\
  return G ? op1 : op0;\
}
__ANY(__iec_)
#undef __iec_

/* Overloaded function */
#define __iec_(TYPENAME)\
static inline TYPENAME SEL__##TYPENAME##__BOOL__##TYPENAME##__##TYPENAME(EN_ENO_PARAMS BOOL G, TYPENAME op0, TYPENAME op1){\
  TEST_EN(TYPENAME)\
  return G ? op1 : op0;\
}
__ANY(__iec_)
#undef __iec_


    /**************/
    /*     MAX    */
    /**************/

#define __extrem_(fname,TYPENAME, COND) \
static inline TYPENAME fname(EN_ENO_PARAMS UINT param_count, TYPENAME op1, ...){\
  va_list ap;\
  UINT i;\
  TEST_EN(TYPENAME)\
  \
  va_start (ap, op1);         /* Initialize the argument list.  */\
  \
  for (i = 0; i < param_count - 1; i++){\
    TYPENAME tmp = va_arg (ap, VA_ARGS_##TYPENAME);\
    op1 = COND ? tmp : op1;\
  }\
  \
  va_end (ap);                  /* Clean up.  */\
  return op1;\
}

/* Max for numerical data types */	
#define __iec_(TYPENAME) \
__extrem_(MAX_##TYPENAME,TYPENAME, op1 < tmp) /* The explicitly typed standard functions */\
__extrem_(MAX__##TYPENAME##__##TYPENAME,TYPENAME, op1 < tmp) /* Overloaded function */
__ANY_BIT(__iec_)
__ANY_NUM(__iec_)
#undef __iec_

/* Max for time data types */	
#define __iec_(TYPENAME) \
__extrem_(MAX_##TYPENAME, TYPENAME, __time_cmp(op1, tmp) < 0) /* The explicitly typed standard functions */\
__extrem_(MAX__##TYPENAME##__##TYPENAME, TYPENAME, __time_cmp(op1, tmp) < 0) /* Overloaded function */
__ANY_DATE(__iec_)
__iec_(TIME)
#undef __iec_

#define __STR_CMP(str1, str2) memcmp((char*)&str1.body,(char*)&str2.body, str1.len < str2.len ? str1.len : str2.len)

/* Max for string data types */	
__extrem_(MAX_STRING, STRING, __STR_CMP(op1,tmp) < 0) /* The explicitly typed standard functions */
__extrem_(MAX__STRING__STRING, STRING, __STR_CMP(op1,tmp) < 0) /* Overloaded function */

    /**************/
    /*     MIN    */
    /**************/
/* Min for numerical data types */	
#define __iec_(TYPENAME) \
__extrem_(MIN_##TYPENAME, TYPENAME, op1 > tmp) /* The explicitly typed standard functions */\
__extrem_(MIN__##TYPENAME##__##TYPENAME, TYPENAME, op1 > tmp) /* Overloaded function */
__ANY_NBIT(__iec_)
__ANY_NUM(__iec_)
#undef __iec_

/* Min for time data types */	
#define __iec_(TYPENAME) \
__extrem_(MIN_##TYPENAME, TYPENAME, __time_cmp(op1, tmp) > 0) /* The explicitly typed standard functions */\
__extrem_(MIN__##TYPENAME##__##TYPENAME, TYPENAME, __time_cmp(op1, tmp) > 0) /* Overloaded function */
__ANY_DATE(__iec_)
__iec_(TIME)
#undef __iec_

/* Min for string data types */
__extrem_(MIN_STRING, STRING, __STR_CMP(op1,tmp) > 0) /* The explicitly typed standard functions */
__extrem_(MIN__STRING__STRING, STRING, __STR_CMP(op1,tmp) > 0) /* Overloaded function */

    /**************/
    /*   LIMIT    */
    /**************/

/* Limit for numerical data types */
#define __iec_(TYPENAME)\
/* The explicitly typed standard functions */\
static inline TYPENAME LIMIT_##TYPENAME(EN_ENO_PARAMS TYPENAME MN, TYPENAME IN, TYPENAME MX){\
  TEST_EN(TYPENAME)\
  return IN > MN ? IN < MX ? IN : MX : MN;\
}\
/* Overloaded function */\
static inline TYPENAME LIMIT__##TYPENAME##__##TYPENAME##__##TYPENAME##__##TYPENAME(EN_ENO_PARAMS TYPENAME MN, TYPENAME IN, TYPENAME MX){\
  TEST_EN(TYPENAME)\
  return IN > MN ? IN < MX ? IN : MX : MN;\
}
__ANY_NBIT(__iec_)
__ANY_NUM(__iec_)
#undef __iec_


/* Limit for time data types */	
#define __iec_(TYPENAME)\
/* The explicitly typed standard functions */\
static inline TYPENAME LIMIT_##TYPENAME(EN_ENO_PARAMS TYPENAME MN, TYPENAME IN, TYPENAME MX){\
    TEST_EN(TYPENAME)\
    return __time_cmp(IN, MN) > 0 ? /* IN>MN ?*/\
           __time_cmp(IN, MX) < 0 ? /* IN<MX ?*/\
           IN : MX : MN;\
}\
/* Overloaded function */\
static inline TYPENAME LIMIT__##TYPENAME##__##TYPENAME##__##TYPENAME##__##TYPENAME(EN_ENO_PARAMS TYPENAME MN, TYPENAME IN, TYPENAME MX){\
    TEST_EN(TYPENAME)\
    return __time_cmp(IN, MN) > 0 ? /* IN>MN ?*/\
           __time_cmp(IN, MX) < 0 ? /* IN<MX ?*/\
           IN : MX : MN;\
}

__ANY_DATE(__iec_)
__iec_(TIME)
#undef __iec_

/* Limit for string data types */	
/* The explicitly typed standard functions */
static inline STRING LIMIT_STRING(EN_ENO_PARAMS STRING MN, STRING IN, STRING MX){
    TEST_EN(STRING)
    return __STR_CMP(IN, MN) > 0 ? __STR_CMP(IN, MX) < 0 ? IN : MX : MN;
}

/* Overloaded function */
static inline STRING LIMIT__STRING__STRING__STRING__STRING(EN_ENO_PARAMS STRING MN, STRING IN, STRING MX){
    TEST_EN(STRING)
    return __STR_CMP(IN, MN) > 0 ? __STR_CMP(IN, MX) < 0 ? IN : MX : MN;
}


    /**************/
    /*     MUX    */
    /**************/
/* The standard states that the inputs for SEL and MUX must be named starting off from 0,
 * unlike remaining functions, that start off at 1.
 */    
/* The explicitly typed standard functions */
#define __in1_anyint_(in2_TYPENAME)   __ANY_INT_1(__iec_,in2_TYPENAME)
#define __iec_(in1_TYPENAME,in2_TYPENAME) \
static inline in2_TYPENAME MUX__##in2_TYPENAME##__##in1_TYPENAME##__##in2_TYPENAME(EN_ENO_PARAMS in1_TYPENAME K, UINT param_count, ...){\
  va_list ap;\
  UINT i;\
  in2_TYPENAME tmp;\
  TEST_EN_COND(in2_TYPENAME, K >= param_count)\
  tmp = __INIT_##in2_TYPENAME;\
  \
  va_start (ap, param_count);         /* Initialize the argument list.  */\
  \
  for (i = 0; i < param_count; i++){\
    if(K == i){\
        tmp = va_arg (ap, VA_ARGS_##in2_TYPENAME);\
        va_end (ap);                  /* Clean up.  */\
        return tmp;\
    }else{\
        va_arg (ap, VA_ARGS_##in2_TYPENAME);\
    }\
  }\
  \
  va_end (ap);                  /* Clean up.  */\
  return tmp;\
}

__ANY(__in1_anyint_)
#undef __iec_


/******************************************/
/***             Table 28               ***/
/***   Standard comparison functions    ***/
/******************************************/

#define __compare_(fname,TYPENAME, COND) \
static inline BOOL fname(EN_ENO_PARAMS UINT param_count, TYPENAME op1, ...){\
  va_list ap;\
  UINT i;\
  TEST_EN(BOOL)\
  \
  va_start (ap, op1);         /* Initialize the argument list.  */\
  DBG(#fname #TYPENAME "\n")\
  DBG_TYPE(TYPENAME, op1)\
  \
  for (i = 0; i < param_count - 1; i++){\
    TYPENAME tmp = va_arg (ap, VA_ARGS_##TYPENAME);\
    DBG_TYPE(TYPENAME, tmp)\
    if(COND){\
        op1 = tmp;\
    }else{\
        va_end (ap);                  /* Clean up.  */\
        return 0;\
    }\
  }\
  \
  va_end (ap);                  /* Clean up.  */\
  return 1;\
}

#define __compare_num(fname, TYPENAME, TEST) __compare_(fname, TYPENAME, op1 TEST tmp )
#define __compare_time(fname, TYPENAME, TEST) __compare_(fname, TYPENAME, __time_cmp(op1, tmp) TEST 0)
#define __compare_string(fname, TEST) __compare_(fname, STRING, __STR_CMP(op1, tmp) TEST 0 )


    /**************/
    /*     GT     */
    /**************/
/* Comparison for numerical data types */
#define __iec_(TYPENAME) \
__compare_num(GT_##TYPENAME, TYPENAME, > ) /* The explicitly typed standard functions */\
__compare_num(GT__BOOL__##TYPENAME, TYPENAME, > ) /* Overloaded function */
__ANY_NBIT(__iec_)
__ANY_NUM(__iec_)
#undef __iec_

/* Comparison for time data types */	
#define __iec_(TYPENAME) \
__compare_time(GT_##TYPENAME, TYPENAME, > ) /* The explicitly typed standard functions */\
__compare_time(GT__BOOL__##TYPENAME, TYPENAME, > ) /* Overloaded function */
__ANY_DATE(__iec_)
__iec_(TIME)
#undef __iec_

/* Comparison for string data types */	
__compare_string(GT_STRING, > ) /* The explicitly typed standard functions */
__compare_string(GT__BOOL__STRING, > ) /* Overloaded function */

    /**************/
    /*     GE     */
    /**************/
/* Comparison for numerical data types */
#define __iec_(TYPENAME) \
__compare_num(GE_##TYPENAME, TYPENAME, >= ) /* The explicitly typed standard functions */\
__compare_num(GE__BOOL__##TYPENAME, TYPENAME, >= ) /* Overloaded function */
__ANY_NBIT(__iec_)
__ANY_NUM(__iec_)
#undef __iec_

/* Comparison for time data types */	
#define __iec_(TYPENAME) \
__compare_time(GE_##TYPENAME, TYPENAME, >= ) /* The explicitly typed standard functions */\
__compare_time(GE__BOOL__##TYPENAME, TYPENAME, >= ) /* Overloaded function */
__ANY_DATE(__iec_)
__iec_(TIME)
#undef __iec_

/* Comparison for string data types */	
__compare_string(GE_STRING, >= ) /* The explicitly typed standard functions */
__compare_string(GE__BOOL__STRING, >= ) /* Overloaded function */



    /**************/
    /*     EQ     */
    /**************/
/* Comparison for numerical data types */
#define __iec_(TYPENAME) \
__compare_num(EQ_##TYPENAME, TYPENAME, == ) /* The explicitly typed standard functions */\
__compare_num(EQ__BOOL__##TYPENAME, TYPENAME, == ) /* Overloaded function */
__ANY_NBIT(__iec_)
__ANY_NUM(__iec_)
#undef __iec_

/* Comparison for time data types */	
#define __iec_(TYPENAME) \
__compare_time(EQ_##TYPENAME, TYPENAME, == ) /* The explicitly typed standard functions */\
__compare_time(EQ__BOOL__##TYPENAME, TYPENAME, == ) /* Overloaded function */
__ANY_DATE(__iec_)
__iec_(TIME)
#undef __iec_

/* Comparison for string data types */	
__compare_string(EQ_STRING, == ) /* The explicitly typed standard functions */
__compare_string(EQ__BOOL__STRING, == ) /* Overloaded function */


    /**************/
    /*     LT     */
    /**************/
/* Comparison for numerical data types */
#define __iec_(TYPENAME) \
__compare_num(LT_##TYPENAME, TYPENAME, < ) /* The explicitly typed standard functions */\
__compare_num(LT__BOOL__##TYPENAME, TYPENAME, < ) /* Overloaded function */
__ANY_NBIT(__iec_)
__ANY_NUM(__iec_)
#undef __iec_

/* Comparison for time data types */	
#define __iec_(TYPENAME) \
__compare_time(LT_##TYPENAME, TYPENAME, < ) /* The explicitly typed standard functions */\
__compare_time(LT__BOOL__##TYPENAME, TYPENAME, < ) /* Overloaded function */
__ANY_DATE(__iec_)
__iec_(TIME)
#undef __iec_

/* Comparison for string data types */	
__compare_string(LT_STRING, < ) /* The explicitly typed standard functions */
__compare_string(LT__BOOL__STRING, < ) /* Overloaded function */


    /**************/
    /*     LE     */
    /**************/
/* Comparison for numerical data types */
#define __iec_(TYPENAME) \
__compare_num(LE_##TYPENAME, TYPENAME, <= ) /* The explicitly typed standard functions */\
__compare_num(LE__BOOL__##TYPENAME, TYPENAME, <= ) /* Overloaded function */
__ANY_NBIT(__iec_)
__ANY_NUM(__iec_)
#undef __iec_

/* Comparison for time data types */	
#define __iec_(TYPENAME) \
__compare_time(LE_##TYPENAME, TYPENAME, <= ) /* The explicitly typed standard functions */\
__compare_time(LE__BOOL__##TYPENAME, TYPENAME, <= ) /* Overloaded function */
__ANY_DATE(__iec_)
__iec_(TIME)
#undef __iec_

/* Comparison for string data types */	
__compare_string(LE_STRING, <= ) /* The explicitly typed standard functions */
__compare_string(LE__BOOL__STRING, <= ) /* Overloaded function */


    /**************/
    /*     NE     */
    /**************/
#define __ne_num(fname, TYPENAME) \
static inline BOOL fname(EN_ENO_PARAMS TYPENAME op1, TYPENAME op2){\
  TEST_EN(BOOL)\
  return op1 != op2 ? 1 : 0;\
}

#define __ne_time(fname, TYPENAME) \
static inline BOOL fname(EN_ENO_PARAMS TYPENAME op1, TYPENAME op2){\
  TEST_EN(BOOL)\
  return __time_cmp(op1, op2) != 0 ? 1 : 0;\
}

#define __ne_string(fname, TYPENAME) \
static inline BOOL fname(EN_ENO_PARAMS TYPENAME op1, TYPENAME op2){\
  TEST_EN(BOOL)\
  return __STR_CMP(op1, op2) != 0 ? 1 : 0;\
}

/* Comparison for numerical data types */
#define __iec_(TYPENAME) \
__ne_num(NE_##TYPENAME, TYPENAME) /* The explicitly typed standard functions */\
__ne_num(NE__BOOL__##TYPENAME##__##TYPENAME, TYPENAME) /* Overloaded function */
__ANY_NBIT(__iec_)
__ANY_NUM(__iec_)
#undef __iec_

/* Comparison for time data types */	
#define __iec_(TYPENAME) \
__ne_time(NE_##TYPENAME, TYPENAME) /* The explicitly typed standard functions */\
__ne_time(NE__BOOL__##TYPENAME##__##TYPENAME, TYPENAME) /* Overloaded function */
__ANY_DATE(__iec_)
__iec_(TIME)
#undef __iec_

/* Comparison for string data types */	
__ne_string(NE_STRING, STRING) /* The explicitly typed standard functions */
__ne_string(NE__BOOL__STRING__STRING, STRING) /* Overloaded function */






/*********************************************/  
/*********************************************/  
/*  2.5.1.5.5   Character string  Functions  */
/*********************************************/  
/*********************************************/  

/*************************************/
/***           Table 29            ***/
/***  Character string  Functions  ***/
/*************************************/

/* We do not delcare explcitly typed versions of the functions in table 29.
 * See note above regarding explicitly typed functions for more details.
 */
 



    /***************/
    /*     LEN     */
    /***************/
static inline __strlen_t __len(STRING IN) {return IN.len;}

/* A function, with 1 input paramter, implementing a generic OPERATION */
#define __genoper_1p_(fname,ret_TYPENAME, par_TYPENAME, OPERATION) \
static inline ret_TYPENAME fname(EN_ENO_PARAMS par_TYPENAME par1){\
  TEST_EN(ret_TYPENAME)\
  return (ret_TYPENAME)OPERATION(par1);\
}

#define __iec_(TYPENAME) __genoper_1p_(LEN__##TYPENAME##__STRING, TYPENAME, STRING, __len)
__ANY_INT(__iec_)
#undef __iec_


    /****************/
    /*     LEFT     */
    /****************/

#define __left(TYPENAME) \
static inline STRING LEFT__STRING__STRING__##TYPENAME(EN_ENO_PARAMS STRING IN, TYPENAME L){\
    STRING res;\
    TEST_EN_COND(STRING, L < 0)\
    res = __INIT_STRING;\
    L = L < (TYPENAME)IN.len ? L : (TYPENAME)IN.len;\
    memcpy(&res.body, &IN.body, (size_t)L);\
    res.len = (__strlen_t)L;\
    return res;\
}
__ANY_INT(__left)


    /*****************/
    /*     RIGHT     */
    /*****************/

#define __right(TYPENAME) \
static inline STRING RIGHT__STRING__STRING__##TYPENAME(EN_ENO_PARAMS STRING IN, TYPENAME L){\
  STRING res;\
  TEST_EN_COND(STRING, L < 0)\
  res = __INIT_STRING;\
  L = L < (TYPENAME)IN.len ? L : (TYPENAME)IN.len;\
  memcpy(&res.body, &IN.body[(TYPENAME)IN.len - L], (size_t)L);\
  res.len = (__strlen_t)L;\
  return res;\
}
__ANY_INT(__right)


    /***************/
    /*     MID     */
    /***************/

#define __mid(TYPENAME) \
static inline STRING MID__STRING__STRING__##TYPENAME##__##TYPENAME(EN_ENO_PARAMS STRING IN, TYPENAME L, TYPENAME P){\
  STRING res;\
  TEST_EN_COND(STRING, L < 0 || P < 0)\
  res = __INIT_STRING;\
  if(P <= (TYPENAME)IN.len){\
	P -= 1; /* now can be used as [index]*/\
	L = L + P <= (TYPENAME)IN.len ? L : (TYPENAME)IN.len - P;\
	memcpy(&res.body, &IN.body[P] , (size_t)L);\
	res.len = (__strlen_t)L;\
  }\
  return res;\
}
__ANY_INT(__mid)


    /******************/
    /*     CONCAT     */
    /******************/

static inline STRING CONCAT(EN_ENO_PARAMS UINT param_count, ...){
  UINT i;
  STRING res;
  va_list ap;
  __strlen_t charcount;
  TEST_EN(STRING)
  charcount = 0;
  res = __INIT_STRING;

  va_start (ap, param_count);         /* Initialize the argument list.  */

  for (i = 0; i < param_count && charcount < STR_MAX_LEN; i++)
  {
    STRING tmp = va_arg(ap, STRING);
    __strlen_t charrem = STR_MAX_LEN - charcount;
    __strlen_t to_write = tmp.len > charrem ? charrem : tmp.len;
    memcpy(&res.body[charcount], &tmp.body , to_write);
    charcount += to_write;
  }

  res.len = charcount;

  va_end (ap);                  /* Clean up.  */
  return res;
}

    /******************/
    /*     INSERT     */
    /******************/

static inline STRING __insert(STRING IN1, STRING IN2, __strlen_t P){
    STRING res;
    __strlen_t to_copy;
    res = __INIT_STRING;

    to_copy = P > IN1.len ? IN1.len : P;
    memcpy(&res.body, &IN1.body , to_copy);
    P = res.len = to_copy;

    to_copy = IN2.len + res.len > STR_MAX_LEN ? STR_MAX_LEN - res.len : IN2.len;
    memcpy(&res.body[res.len], &IN2.body , to_copy);
    res.len += to_copy;

    to_copy = IN1.len - P < STR_MAX_LEN - res.len ? IN1.len - P : STR_MAX_LEN - res.len ;
    memcpy(&res.body[res.len], &IN1.body[P] , to_copy);
    res.len += to_copy;

    return res;
}

#define __iec_(TYPENAME) \
static inline STRING INSERT__STRING__STRING__STRING__##TYPENAME(EN_ENO_PARAMS STRING str1, STRING str2, TYPENAME P){\
  TEST_EN_COND(STRING, P < 0)\
  return (STRING)__insert(str1,str2,(__strlen_t)P);\
}
__ANY_INT(__iec_)
#undef __iec_


    /******************/
    /*     DELETE     */
    /******************/

static inline STRING __delete(STRING IN, __strlen_t L, __strlen_t P){
    STRING res;
    __strlen_t to_copy;
    res = __INIT_STRING;

    to_copy = P > IN.len ? IN.len : P-1;
    memcpy(&res.body, &IN.body , to_copy);
    P = res.len = to_copy;

    if( IN.len > P + L ){
        to_copy = IN.len - P - L;
        memcpy(&res.body[res.len], &IN.body[P + L], to_copy);
        res.len += to_copy;
    }

    return res;
}

#define __iec_(TYPENAME) \
static inline STRING DELETE__STRING__STRING__##TYPENAME##__##TYPENAME(EN_ENO_PARAMS STRING str, TYPENAME L, TYPENAME P){\
  TEST_EN_COND(STRING, L < 0 || P < 0)\
  return (STRING)__delete(str,(__strlen_t)L,(__strlen_t)P);\
}
__ANY_INT(__iec_)
#undef __iec_


    /*******************/
    /*     REPLACE     */
    /*******************/

static inline STRING __replace(STRING IN1, STRING IN2, __strlen_t L, __strlen_t P){
    STRING res;
    __strlen_t to_copy;
    res = __INIT_STRING;

    to_copy = P > IN1.len ? IN1.len : P-1;
    memcpy(&res.body, &IN1.body , to_copy);
    P = res.len = to_copy;

    to_copy = IN2.len < L ? IN2.len : L;

    if( to_copy + res.len > STR_MAX_LEN )
       to_copy = STR_MAX_LEN - res.len;

    memcpy(&res.body[res.len], &IN2.body , to_copy);
    res.len += to_copy;

    P += L;
    if( res.len <  STR_MAX_LEN && P < IN1.len)
    {
        to_copy = IN1.len - P;
        memcpy(&res.body[res.len], &IN1.body[P] , to_copy);
        res.len += to_copy;
    }

    return res;
}

#define __iec_(TYPENAME) \
static inline STRING REPLACE__STRING__STRING__STRING__##TYPENAME##__##TYPENAME(EN_ENO_PARAMS STRING str1, STRING str2, TYPENAME L, TYPENAME P){\
  TEST_EN_COND(STRING, L < 0 || P < 0)\
  return (STRING)__replace(str1,str2,(__strlen_t)L,(__strlen_t)P);\
}
__ANY_INT(__iec_)
#undef __iec_

    /****************/
    /*     FIND     */
    /****************/

static inline __strlen_t __pfind(STRING* IN1, STRING* IN2){
    UINT count1 = 0; /* offset of first matching char in IN1 */
    UINT count2 = 0; /* count of matching char */
    while(count1 + count2 < IN1->len && count2 < IN2->len)
    {
        if(IN1->body[count1 + count2] != IN2->body[count2]){
            count1 += count2 + 1;
            count2 = 0;
        }
        else {
            count2++;
        }
    }
    return count2 == IN2->len -1 ? 0 : count1 + 1;
}

#define __iec_(TYPENAME) \
static inline TYPENAME FIND__##TYPENAME##__STRING__STRING(EN_ENO_PARAMS STRING str1, STRING str2){\
  TEST_EN(TYPENAME)\
  return (TYPENAME)__pfind(&str1,&str2);\
}
__ANY_INT(__iec_)
#undef __iec_


/*********************************************/  
/*********************************************/  
/*  2.5.1.5.6  Functions of time data types  */
/*********************************************/  
/*********************************************/  

/**************************************/
/***           Table 30             ***/
/***  Functions of time data types  ***/
/**************************************/


static inline TIME ADD_TIME(EN_ENO_PARAMS TIME IN1, TIME IN2){
  TEST_EN(TIME)
  return __time_add(IN1, IN2);
}

static inline TOD ADD_TOD_TIME(EN_ENO_PARAMS TOD IN1, TIME IN2){
  TEST_EN(TOD)
  return __time_add(IN1, IN2);
}

static inline DT ADD_DT_TIME(EN_ENO_PARAMS DT IN1, TIME IN2){
  TEST_EN(DT)
  return __time_add(IN1, IN2);
}

static inline TIME SUB_TIME(EN_ENO_PARAMS TIME IN1, TIME IN2){
  TEST_EN(TIME)
  return __time_sub(IN1, IN2);
}

static inline TIME SUB_DATE_DATE(EN_ENO_PARAMS DATE IN1, DATE IN2){
  TEST_EN(TIME)
  return __time_sub(IN1, IN2);
}

static inline TOD SUB_TOD_TIME(EN_ENO_PARAMS TOD IN1, TIME IN2){
  TEST_EN(TOD)
  return __time_sub(IN1, IN2);
}

static inline TIME SUB_TOD_TOD(EN_ENO_PARAMS TOD IN1, TOD IN2){
  TEST_EN(TIME)
  return __time_sub(IN1, IN2);
}

static inline DT SUB_DT_TIME(EN_ENO_PARAMS DT IN1, TIME IN2){
  TEST_EN(DT)
  return __time_sub(IN1, IN2);
}

static inline TIME SUB_DT_DT(EN_ENO_PARAMS DT IN1, DT IN2){
  TEST_EN(TIME)
  return __time_sub(IN1, IN2);
}


/***  MULTIME  ***/
#define __iec_(TYPENAME)\
static inline TIME MULTIME__TIME__TIME__##TYPENAME(EN_ENO_PARAMS TIME IN1, TYPENAME IN2){\
  TEST_EN(TIME)\
  return __time_mul(IN1, (LREAL)IN2);\
}
__ANY_NUM(__iec_)
#undef __iec_

/***  MUL  ***/
#define __iec_(TYPENAME)\
static inline TIME MUL__TIME__TIME__##TYPENAME(EN_ENO_PARAMS TIME IN1, TYPENAME IN2){\
  TEST_EN(TIME)\
  return __time_mul(IN1, (LREAL)IN2);\
}
__ANY_NUM(__iec_)
#undef __iec_

/***  DIVTIME  ***/
#define __iec_(TYPENAME)\
static inline TIME DIVTIME__TIME__TIME__##TYPENAME(EN_ENO_PARAMS TIME IN1, TYPENAME IN2){\
  TEST_EN(TIME)\
  return __time_div(IN1, (LREAL)IN2);\
}
__ANY_NUM(__iec_)
#undef __iec_

/***  DIV  ***/
#define __iec_(TYPENAME)\
static inline TIME DIV__TIME__TIME__##TYPENAME(EN_ENO_PARAMS TIME IN1, TYPENAME IN2){\
  TEST_EN(TIME)\
  return __time_div(IN1, (LREAL)IN2);\
}
__ANY_NUM(__iec_)
#undef __iec_

/*** CONCAT_DATE_TOD ***/
static inline DT CONCAT_DATE_TOD(EN_ENO_PARAMS DATE IN1, TOD IN2){
  TEST_EN(DT)
  return __time_add(IN1, IN2);
}



/****************************************************/  
/****************************************************/  
/*  2.5.1.5.6   Functions of enumerated data types  */
/****************************************************/  
/****************************************************/  

/********************************************/
/***              Table 31                ***/
/***  Functions of enumerated data types  ***/
/********************************************/

/* Do we support this? */




/*************************************/
/*
  These defines are needed to have #if-like
  behaviour inside of macro. See macro
  ARITH_OPERATION_COND above.

  emacs commands to generate defines below.
  Select commands and eval-region

  (next-line)
  (next-line)
  (dotimes (i 200)
    (insert
     (format "\n#define ARITH_OPERATION_COND_%d_LE_8 %s" i (if (> i 8) "," "1"))))
*/

#define ARITH_OPERATION_COND_0_LE_8 1
#define ARITH_OPERATION_COND_1_LE_8 1
#define ARITH_OPERATION_COND_2_LE_8 1
#define ARITH_OPERATION_COND_3_LE_8 1
#define ARITH_OPERATION_COND_4_LE_8 1
#define ARITH_OPERATION_COND_5_LE_8 1
#define ARITH_OPERATION_COND_6_LE_8 1
#define ARITH_OPERATION_COND_7_LE_8 1
#define ARITH_OPERATION_COND_8_LE_8 1
#define ARITH_OPERATION_COND_9_LE_8 ,
#define ARITH_OPERATION_COND_10_LE_8 ,
#define ARITH_OPERATION_COND_11_LE_8 ,
#define ARITH_OPERATION_COND_12_LE_8 ,
#define ARITH_OPERATION_COND_13_LE_8 ,
#define ARITH_OPERATION_COND_14_LE_8 ,
#define ARITH_OPERATION_COND_15_LE_8 ,
#define ARITH_OPERATION_COND_16_LE_8 ,
#define ARITH_OPERATION_COND_17_LE_8 ,
#define ARITH_OPERATION_COND_18_LE_8 ,
#define ARITH_OPERATION_COND_19_LE_8 ,
#define ARITH_OPERATION_COND_20_LE_8 ,
#define ARITH_OPERATION_COND_21_LE_8 ,
#define ARITH_OPERATION_COND_22_LE_8 ,
#define ARITH_OPERATION_COND_23_LE_8 ,
#define ARITH_OPERATION_COND_24_LE_8 ,
#define ARITH_OPERATION_COND_25_LE_8 ,
#define ARITH_OPERATION_COND_26_LE_8 ,
#define ARITH_OPERATION_COND_27_LE_8 ,
#define ARITH_OPERATION_COND_28_LE_8 ,
#define ARITH_OPERATION_COND_29_LE_8 ,
#define ARITH_OPERATION_COND_30_LE_8 ,
#define ARITH_OPERATION_COND_31_LE_8 ,
#define ARITH_OPERATION_COND_32_LE_8 ,
#define ARITH_OPERATION_COND_33_LE_8 ,
#define ARITH_OPERATION_COND_34_LE_8 ,
#define ARITH_OPERATION_COND_35_LE_8 ,
#define ARITH_OPERATION_COND_36_LE_8 ,
#define ARITH_OPERATION_COND_37_LE_8 ,
#define ARITH_OPERATION_COND_38_LE_8 ,
#define ARITH_OPERATION_COND_39_LE_8 ,
#define ARITH_OPERATION_COND_40_LE_8 ,
#define ARITH_OPERATION_COND_41_LE_8 ,
#define ARITH_OPERATION_COND_42_LE_8 ,
#define ARITH_OPERATION_COND_43_LE_8 ,
#define ARITH_OPERATION_COND_44_LE_8 ,
#define ARITH_OPERATION_COND_45_LE_8 ,
#define ARITH_OPERATION_COND_46_LE_8 ,
#define ARITH_OPERATION_COND_47_LE_8 ,
#define ARITH_OPERATION_COND_48_LE_8 ,
#define ARITH_OPERATION_COND_49_LE_8 ,
#define ARITH_OPERATION_COND_50_LE_8 ,
#define ARITH_OPERATION_COND_51_LE_8 ,
#define ARITH_OPERATION_COND_52_LE_8 ,
#define ARITH_OPERATION_COND_53_LE_8 ,
#define ARITH_OPERATION_COND_54_LE_8 ,
#define ARITH_OPERATION_COND_55_LE_8 ,
#define ARITH_OPERATION_COND_56_LE_8 ,
#define ARITH_OPERATION_COND_57_LE_8 ,
#define ARITH_OPERATION_COND_58_LE_8 ,
#define ARITH_OPERATION_COND_59_LE_8 ,
#define ARITH_OPERATION_COND_60_LE_8 ,
#define ARITH_OPERATION_COND_61_LE_8 ,
#define ARITH_OPERATION_COND_62_LE_8 ,
#define ARITH_OPERATION_COND_63_LE_8 ,
#define ARITH_OPERATION_COND_64_LE_8 ,
#define ARITH_OPERATION_COND_65_LE_8 ,
#define ARITH_OPERATION_COND_66_LE_8 ,
#define ARITH_OPERATION_COND_67_LE_8 ,
#define ARITH_OPERATION_COND_68_LE_8 ,
#define ARITH_OPERATION_COND_69_LE_8 ,
#define ARITH_OPERATION_COND_70_LE_8 ,
#define ARITH_OPERATION_COND_71_LE_8 ,
#define ARITH_OPERATION_COND_72_LE_8 ,
#define ARITH_OPERATION_COND_73_LE_8 ,
#define ARITH_OPERATION_COND_74_LE_8 ,
#define ARITH_OPERATION_COND_75_LE_8 ,
#define ARITH_OPERATION_COND_76_LE_8 ,
#define ARITH_OPERATION_COND_77_LE_8 ,
#define ARITH_OPERATION_COND_78_LE_8 ,
#define ARITH_OPERATION_COND_79_LE_8 ,
#define ARITH_OPERATION_COND_80_LE_8 ,
#define ARITH_OPERATION_COND_81_LE_8 ,
#define ARITH_OPERATION_COND_82_LE_8 ,
#define ARITH_OPERATION_COND_83_LE_8 ,
#define ARITH_OPERATION_COND_84_LE_8 ,
#define ARITH_OPERATION_COND_85_LE_8 ,
#define ARITH_OPERATION_COND_86_LE_8 ,
#define ARITH_OPERATION_COND_87_LE_8 ,
#define ARITH_OPERATION_COND_88_LE_8 ,
#define ARITH_OPERATION_COND_89_LE_8 ,
#define ARITH_OPERATION_COND_90_LE_8 ,
#define ARITH_OPERATION_COND_91_LE_8 ,
#define ARITH_OPERATION_COND_92_LE_8 ,
#define ARITH_OPERATION_COND_93_LE_8 ,
#define ARITH_OPERATION_COND_94_LE_8 ,
#define ARITH_OPERATION_COND_95_LE_8 ,
#define ARITH_OPERATION_COND_96_LE_8 ,
#define ARITH_OPERATION_COND_97_LE_8 ,
#define ARITH_OPERATION_COND_98_LE_8 ,
#define ARITH_OPERATION_COND_99_LE_8 ,
#define ARITH_OPERATION_COND_100_LE_8 ,
#define ARITH_OPERATION_COND_101_LE_8 ,
#define ARITH_OPERATION_COND_102_LE_8 ,
#define ARITH_OPERATION_COND_103_LE_8 ,
#define ARITH_OPERATION_COND_104_LE_8 ,
#define ARITH_OPERATION_COND_105_LE_8 ,
#define ARITH_OPERATION_COND_106_LE_8 ,
#define ARITH_OPERATION_COND_107_LE_8 ,
#define ARITH_OPERATION_COND_108_LE_8 ,
#define ARITH_OPERATION_COND_109_LE_8 ,
#define ARITH_OPERATION_COND_110_LE_8 ,
#define ARITH_OPERATION_COND_111_LE_8 ,
#define ARITH_OPERATION_COND_112_LE_8 ,
#define ARITH_OPERATION_COND_113_LE_8 ,
#define ARITH_OPERATION_COND_114_LE_8 ,
#define ARITH_OPERATION_COND_115_LE_8 ,
#define ARITH_OPERATION_COND_116_LE_8 ,
#define ARITH_OPERATION_COND_117_LE_8 ,
#define ARITH_OPERATION_COND_118_LE_8 ,
#define ARITH_OPERATION_COND_119_LE_8 ,
#define ARITH_OPERATION_COND_120_LE_8 ,
#define ARITH_OPERATION_COND_121_LE_8 ,
#define ARITH_OPERATION_COND_122_LE_8 ,
#define ARITH_OPERATION_COND_123_LE_8 ,
#define ARITH_OPERATION_COND_124_LE_8 ,
#define ARITH_OPERATION_COND_125_LE_8 ,
#define ARITH_OPERATION_COND_126_LE_8 ,
#define ARITH_OPERATION_COND_127_LE_8 ,
#define ARITH_OPERATION_COND_128_LE_8 ,
#define ARITH_OPERATION_COND_129_LE_8 ,
#define ARITH_OPERATION_COND_130_LE_8 ,
#define ARITH_OPERATION_COND_131_LE_8 ,
#define ARITH_OPERATION_COND_132_LE_8 ,
#define ARITH_OPERATION_COND_133_LE_8 ,
#define ARITH_OPERATION_COND_134_LE_8 ,
#define ARITH_OPERATION_COND_135_LE_8 ,
#define ARITH_OPERATION_COND_136_LE_8 ,
#define ARITH_OPERATION_COND_137_LE_8 ,
#define ARITH_OPERATION_COND_138_LE_8 ,
#define ARITH_OPERATION_COND_139_LE_8 ,
#define ARITH_OPERATION_COND_140_LE_8 ,
#define ARITH_OPERATION_COND_141_LE_8 ,
#define ARITH_OPERATION_COND_142_LE_8 ,
#define ARITH_OPERATION_COND_143_LE_8 ,
#define ARITH_OPERATION_COND_144_LE_8 ,
#define ARITH_OPERATION_COND_145_LE_8 ,
#define ARITH_OPERATION_COND_146_LE_8 ,
#define ARITH_OPERATION_COND_147_LE_8 ,
#define ARITH_OPERATION_COND_148_LE_8 ,
#define ARITH_OPERATION_COND_149_LE_8 ,
#define ARITH_OPERATION_COND_150_LE_8 ,
#define ARITH_OPERATION_COND_151_LE_8 ,
#define ARITH_OPERATION_COND_152_LE_8 ,
#define ARITH_OPERATION_COND_153_LE_8 ,
#define ARITH_OPERATION_COND_154_LE_8 ,
#define ARITH_OPERATION_COND_155_LE_8 ,
#define ARITH_OPERATION_COND_156_LE_8 ,
#define ARITH_OPERATION_COND_157_LE_8 ,
#define ARITH_OPERATION_COND_158_LE_8 ,
#define ARITH_OPERATION_COND_159_LE_8 ,
#define ARITH_OPERATION_COND_160_LE_8 ,
#define ARITH_OPERATION_COND_161_LE_8 ,
#define ARITH_OPERATION_COND_162_LE_8 ,
#define ARITH_OPERATION_COND_163_LE_8 ,
#define ARITH_OPERATION_COND_164_LE_8 ,
#define ARITH_OPERATION_COND_165_LE_8 ,
#define ARITH_OPERATION_COND_166_LE_8 ,
#define ARITH_OPERATION_COND_167_LE_8 ,
#define ARITH_OPERATION_COND_168_LE_8 ,
#define ARITH_OPERATION_COND_169_LE_8 ,
#define ARITH_OPERATION_COND_170_LE_8 ,
#define ARITH_OPERATION_COND_171_LE_8 ,
#define ARITH_OPERATION_COND_172_LE_8 ,
#define ARITH_OPERATION_COND_173_LE_8 ,
#define ARITH_OPERATION_COND_174_LE_8 ,
#define ARITH_OPERATION_COND_175_LE_8 ,
#define ARITH_OPERATION_COND_176_LE_8 ,
#define ARITH_OPERATION_COND_177_LE_8 ,
#define ARITH_OPERATION_COND_178_LE_8 ,
#define ARITH_OPERATION_COND_179_LE_8 ,
#define ARITH_OPERATION_COND_180_LE_8 ,
#define ARITH_OPERATION_COND_181_LE_8 ,
#define ARITH_OPERATION_COND_182_LE_8 ,
#define ARITH_OPERATION_COND_183_LE_8 ,
#define ARITH_OPERATION_COND_184_LE_8 ,
#define ARITH_OPERATION_COND_185_LE_8 ,
#define ARITH_OPERATION_COND_186_LE_8 ,
#define ARITH_OPERATION_COND_187_LE_8 ,
#define ARITH_OPERATION_COND_188_LE_8 ,
#define ARITH_OPERATION_COND_189_LE_8 ,
#define ARITH_OPERATION_COND_190_LE_8 ,
#define ARITH_OPERATION_COND_191_LE_8 ,
#define ARITH_OPERATION_COND_192_LE_8 ,
#define ARITH_OPERATION_COND_193_LE_8 ,
#define ARITH_OPERATION_COND_194_LE_8 ,
#define ARITH_OPERATION_COND_195_LE_8 ,
#define ARITH_OPERATION_COND_196_LE_8 ,
#define ARITH_OPERATION_COND_197_LE_8 ,
#define ARITH_OPERATION_COND_198_LE_8 ,
#define ARITH_OPERATION_COND_199_LE_8 ,
#define ARITH_OPERATION_COND_200_LE_8 ,
#define ARITH_OPERATION_COND_201_LE_8 ,
#define ARITH_OPERATION_COND_202_LE_8 ,
#define ARITH_OPERATION_COND_203_LE_8 ,
#define ARITH_OPERATION_COND_204_LE_8 ,
#define ARITH_OPERATION_COND_205_LE_8 ,
#define ARITH_OPERATION_COND_206_LE_8 ,
#define ARITH_OPERATION_COND_207_LE_8 ,
#define ARITH_OPERATION_COND_208_LE_8 ,
#define ARITH_OPERATION_COND_209_LE_8 ,
#define ARITH_OPERATION_COND_210_LE_8 ,
#define ARITH_OPERATION_COND_211_LE_8 ,
#define ARITH_OPERATION_COND_212_LE_8 ,
#define ARITH_OPERATION_COND_213_LE_8 ,
#define ARITH_OPERATION_COND_214_LE_8 ,
#define ARITH_OPERATION_COND_215_LE_8 ,
#define ARITH_OPERATION_COND_216_LE_8 ,
#define ARITH_OPERATION_COND_217_LE_8 ,
#define ARITH_OPERATION_COND_218_LE_8 ,
#define ARITH_OPERATION_COND_219_LE_8 ,
#define ARITH_OPERATION_COND_220_LE_8 ,
#define ARITH_OPERATION_COND_221_LE_8 ,
#define ARITH_OPERATION_COND_222_LE_8 ,
#define ARITH_OPERATION_COND_223_LE_8 ,
#define ARITH_OPERATION_COND_224_LE_8 ,
#define ARITH_OPERATION_COND_225_LE_8 ,
#define ARITH_OPERATION_COND_226_LE_8 ,
#define ARITH_OPERATION_COND_227_LE_8 ,
#define ARITH_OPERATION_COND_228_LE_8 ,
#define ARITH_OPERATION_COND_229_LE_8 ,
#define ARITH_OPERATION_COND_230_LE_8 ,
#define ARITH_OPERATION_COND_231_LE_8 ,
#define ARITH_OPERATION_COND_232_LE_8 ,
#define ARITH_OPERATION_COND_233_LE_8 ,
#define ARITH_OPERATION_COND_234_LE_8 ,
#define ARITH_OPERATION_COND_235_LE_8 ,
#define ARITH_OPERATION_COND_236_LE_8 ,
#define ARITH_OPERATION_COND_237_LE_8 ,
#define ARITH_OPERATION_COND_238_LE_8 ,
#define ARITH_OPERATION_COND_239_LE_8 ,
#define ARITH_OPERATION_COND_240_LE_8 ,
#define ARITH_OPERATION_COND_241_LE_8 ,
#define ARITH_OPERATION_COND_242_LE_8 ,
#define ARITH_OPERATION_COND_243_LE_8 ,
#define ARITH_OPERATION_COND_244_LE_8 ,
#define ARITH_OPERATION_COND_245_LE_8 ,
#define ARITH_OPERATION_COND_246_LE_8 ,
#define ARITH_OPERATION_COND_247_LE_8 ,
#define ARITH_OPERATION_COND_248_LE_8 ,
#define ARITH_OPERATION_COND_249_LE_8 ,
#define ARITH_OPERATION_COND_250_LE_8 ,
#define ARITH_OPERATION_COND_251_LE_8 ,
#define ARITH_OPERATION_COND_252_LE_8 ,
#define ARITH_OPERATION_COND_253_LE_8 ,
#define ARITH_OPERATION_COND_254_LE_8 ,
#define ARITH_OPERATION_COND_255_LE_8 ,
#define ARITH_OPERATION_COND_256_LE_8 ,
#define ARITH_OPERATION_COND_257_LE_8 ,
#define ARITH_OPERATION_COND_258_LE_8 ,
#define ARITH_OPERATION_COND_259_LE_8 ,
#define ARITH_OPERATION_COND_260_LE_8 ,
#define ARITH_OPERATION_COND_261_LE_8 ,
#define ARITH_OPERATION_COND_262_LE_8 ,
#define ARITH_OPERATION_COND_263_LE_8 ,
#define ARITH_OPERATION_COND_264_LE_8 ,
#define ARITH_OPERATION_COND_265_LE_8 ,
#define ARITH_OPERATION_COND_266_LE_8 ,
#define ARITH_OPERATION_COND_267_LE_8 ,
#define ARITH_OPERATION_COND_268_LE_8 ,
#define ARITH_OPERATION_COND_269_LE_8 ,
#define ARITH_OPERATION_COND_270_LE_8 ,
#define ARITH_OPERATION_COND_271_LE_8 ,
#define ARITH_OPERATION_COND_272_LE_8 ,
#define ARITH_OPERATION_COND_273_LE_8 ,
#define ARITH_OPERATION_COND_274_LE_8 ,
#define ARITH_OPERATION_COND_275_LE_8 ,
#define ARITH_OPERATION_COND_276_LE_8 ,
#define ARITH_OPERATION_COND_277_LE_8 ,
#define ARITH_OPERATION_COND_278_LE_8 ,
#define ARITH_OPERATION_COND_279_LE_8 ,
#define ARITH_OPERATION_COND_280_LE_8 ,
#define ARITH_OPERATION_COND_281_LE_8 ,
#define ARITH_OPERATION_COND_282_LE_8 ,
#define ARITH_OPERATION_COND_283_LE_8 ,
#define ARITH_OPERATION_COND_284_LE_8 ,
#define ARITH_OPERATION_COND_285_LE_8 ,
#define ARITH_OPERATION_COND_286_LE_8 ,
#define ARITH_OPERATION_COND_287_LE_8 ,
#define ARITH_OPERATION_COND_288_LE_8 ,
#define ARITH_OPERATION_COND_289_LE_8 ,
#define ARITH_OPERATION_COND_290_LE_8 ,
#define ARITH_OPERATION_COND_291_LE_8 ,
#define ARITH_OPERATION_COND_292_LE_8 ,
#define ARITH_OPERATION_COND_293_LE_8 ,
#define ARITH_OPERATION_COND_294_LE_8 ,
#define ARITH_OPERATION_COND_295_LE_8 ,
#define ARITH_OPERATION_COND_296_LE_8 ,
#define ARITH_OPERATION_COND_297_LE_8 ,
#define ARITH_OPERATION_COND_298_LE_8 ,
#define ARITH_OPERATION_COND_299_LE_8 ,
#define ARITH_OPERATION_COND_300_LE_8 ,
#define ARITH_OPERATION_COND_301_LE_8 ,
#define ARITH_OPERATION_COND_302_LE_8 ,
#define ARITH_OPERATION_COND_303_LE_8 ,
#define ARITH_OPERATION_COND_304_LE_8 ,
#define ARITH_OPERATION_COND_305_LE_8 ,
#define ARITH_OPERATION_COND_306_LE_8 ,
#define ARITH_OPERATION_COND_307_LE_8 ,
#define ARITH_OPERATION_COND_308_LE_8 ,
#define ARITH_OPERATION_COND_309_LE_8 ,
#define ARITH_OPERATION_COND_310_LE_8 ,
#define ARITH_OPERATION_COND_311_LE_8 ,
#define ARITH_OPERATION_COND_312_LE_8 ,
#define ARITH_OPERATION_COND_313_LE_8 ,
#define ARITH_OPERATION_COND_314_LE_8 ,
#define ARITH_OPERATION_COND_315_LE_8 ,
#define ARITH_OPERATION_COND_316_LE_8 ,
#define ARITH_OPERATION_COND_317_LE_8 ,
#define ARITH_OPERATION_COND_318_LE_8 ,
#define ARITH_OPERATION_COND_319_LE_8 ,
#define ARITH_OPERATION_COND_320_LE_8 ,
#define ARITH_OPERATION_COND_321_LE_8 ,
#define ARITH_OPERATION_COND_322_LE_8 ,
#define ARITH_OPERATION_COND_323_LE_8 ,
#define ARITH_OPERATION_COND_324_LE_8 ,
#define ARITH_OPERATION_COND_325_LE_8 ,
#define ARITH_OPERATION_COND_326_LE_8 ,
#define ARITH_OPERATION_COND_327_LE_8 ,
#define ARITH_OPERATION_COND_328_LE_8 ,
#define ARITH_OPERATION_COND_329_LE_8 ,
#define ARITH_OPERATION_COND_330_LE_8 ,
#define ARITH_OPERATION_COND_331_LE_8 ,
#define ARITH_OPERATION_COND_332_LE_8 ,
#define ARITH_OPERATION_COND_333_LE_8 ,
#define ARITH_OPERATION_COND_334_LE_8 ,
#define ARITH_OPERATION_COND_335_LE_8 ,
#define ARITH_OPERATION_COND_336_LE_8 ,
#define ARITH_OPERATION_COND_337_LE_8 ,
#define ARITH_OPERATION_COND_338_LE_8 ,
#define ARITH_OPERATION_COND_339_LE_8 ,
#define ARITH_OPERATION_COND_340_LE_8 ,
#define ARITH_OPERATION_COND_341_LE_8 ,
#define ARITH_OPERATION_COND_342_LE_8 ,
#define ARITH_OPERATION_COND_343_LE_8 ,
#define ARITH_OPERATION_COND_344_LE_8 ,
#define ARITH_OPERATION_COND_345_LE_8 ,
#define ARITH_OPERATION_COND_346_LE_8 ,
#define ARITH_OPERATION_COND_347_LE_8 ,
#define ARITH_OPERATION_COND_348_LE_8 ,
#define ARITH_OPERATION_COND_349_LE_8 ,
#define ARITH_OPERATION_COND_350_LE_8 ,
#define ARITH_OPERATION_COND_351_LE_8 ,
#define ARITH_OPERATION_COND_352_LE_8 ,
#define ARITH_OPERATION_COND_353_LE_8 ,
#define ARITH_OPERATION_COND_354_LE_8 ,
#define ARITH_OPERATION_COND_355_LE_8 ,
#define ARITH_OPERATION_COND_356_LE_8 ,
#define ARITH_OPERATION_COND_357_LE_8 ,
#define ARITH_OPERATION_COND_358_LE_8 ,
#define ARITH_OPERATION_COND_359_LE_8 ,
#define ARITH_OPERATION_COND_360_LE_8 ,
#define ARITH_OPERATION_COND_361_LE_8 ,
#define ARITH_OPERATION_COND_362_LE_8 ,
#define ARITH_OPERATION_COND_363_LE_8 ,
#define ARITH_OPERATION_COND_364_LE_8 ,
#define ARITH_OPERATION_COND_365_LE_8 ,
#define ARITH_OPERATION_COND_366_LE_8 ,
#define ARITH_OPERATION_COND_367_LE_8 ,
#define ARITH_OPERATION_COND_368_LE_8 ,
#define ARITH_OPERATION_COND_369_LE_8 ,
#define ARITH_OPERATION_COND_370_LE_8 ,
#define ARITH_OPERATION_COND_371_LE_8 ,
#define ARITH_OPERATION_COND_372_LE_8 ,
#define ARITH_OPERATION_COND_373_LE_8 ,
#define ARITH_OPERATION_COND_374_LE_8 ,
#define ARITH_OPERATION_COND_375_LE_8 ,
#define ARITH_OPERATION_COND_376_LE_8 ,
#define ARITH_OPERATION_COND_377_LE_8 ,
#define ARITH_OPERATION_COND_378_LE_8 ,
#define ARITH_OPERATION_COND_379_LE_8 ,
#define ARITH_OPERATION_COND_380_LE_8 ,
#define ARITH_OPERATION_COND_381_LE_8 ,
#define ARITH_OPERATION_COND_382_LE_8 ,
#define ARITH_OPERATION_COND_383_LE_8 ,
#define ARITH_OPERATION_COND_384_LE_8 ,
#define ARITH_OPERATION_COND_385_LE_8 ,
#define ARITH_OPERATION_COND_386_LE_8 ,
#define ARITH_OPERATION_COND_387_LE_8 ,
#define ARITH_OPERATION_COND_388_LE_8 ,
#define ARITH_OPERATION_COND_389_LE_8 ,
#define ARITH_OPERATION_COND_390_LE_8 ,
#define ARITH_OPERATION_COND_391_LE_8 ,
#define ARITH_OPERATION_COND_392_LE_8 ,
#define ARITH_OPERATION_COND_393_LE_8 ,
#define ARITH_OPERATION_COND_394_LE_8 ,
#define ARITH_OPERATION_COND_395_LE_8 ,
#define ARITH_OPERATION_COND_396_LE_8 ,
#define ARITH_OPERATION_COND_397_LE_8 ,
#define ARITH_OPERATION_COND_398_LE_8 ,
#define ARITH_OPERATION_COND_399_LE_8 ,
#define ARITH_OPERATION_COND_400_LE_8 ,
#define ARITH_OPERATION_COND_401_LE_8 ,
#define ARITH_OPERATION_COND_402_LE_8 ,
#define ARITH_OPERATION_COND_403_LE_8 ,
#define ARITH_OPERATION_COND_404_LE_8 ,
#define ARITH_OPERATION_COND_405_LE_8 ,
#define ARITH_OPERATION_COND_406_LE_8 ,
#define ARITH_OPERATION_COND_407_LE_8 ,
#define ARITH_OPERATION_COND_408_LE_8 ,
#define ARITH_OPERATION_COND_409_LE_8 ,
#define ARITH_OPERATION_COND_410_LE_8 ,
#define ARITH_OPERATION_COND_411_LE_8 ,
#define ARITH_OPERATION_COND_412_LE_8 ,
#define ARITH_OPERATION_COND_413_LE_8 ,
#define ARITH_OPERATION_COND_414_LE_8 ,
#define ARITH_OPERATION_COND_415_LE_8 ,
#define ARITH_OPERATION_COND_416_LE_8 ,
#define ARITH_OPERATION_COND_417_LE_8 ,
#define ARITH_OPERATION_COND_418_LE_8 ,
#define ARITH_OPERATION_COND_419_LE_8 ,
#define ARITH_OPERATION_COND_420_LE_8 ,
#define ARITH_OPERATION_COND_421_LE_8 ,
#define ARITH_OPERATION_COND_422_LE_8 ,
#define ARITH_OPERATION_COND_423_LE_8 ,
#define ARITH_OPERATION_COND_424_LE_8 ,
#define ARITH_OPERATION_COND_425_LE_8 ,
#define ARITH_OPERATION_COND_426_LE_8 ,
#define ARITH_OPERATION_COND_427_LE_8 ,
#define ARITH_OPERATION_COND_428_LE_8 ,
#define ARITH_OPERATION_COND_429_LE_8 ,
#define ARITH_OPERATION_COND_430_LE_8 ,
#define ARITH_OPERATION_COND_431_LE_8 ,
#define ARITH_OPERATION_COND_432_LE_8 ,
#define ARITH_OPERATION_COND_433_LE_8 ,
#define ARITH_OPERATION_COND_434_LE_8 ,
#define ARITH_OPERATION_COND_435_LE_8 ,
#define ARITH_OPERATION_COND_436_LE_8 ,
#define ARITH_OPERATION_COND_437_LE_8 ,
#define ARITH_OPERATION_COND_438_LE_8 ,
#define ARITH_OPERATION_COND_439_LE_8 ,
#define ARITH_OPERATION_COND_440_LE_8 ,
#define ARITH_OPERATION_COND_441_LE_8 ,
#define ARITH_OPERATION_COND_442_LE_8 ,
#define ARITH_OPERATION_COND_443_LE_8 ,
#define ARITH_OPERATION_COND_444_LE_8 ,
#define ARITH_OPERATION_COND_445_LE_8 ,
#define ARITH_OPERATION_COND_446_LE_8 ,
#define ARITH_OPERATION_COND_447_LE_8 ,
#define ARITH_OPERATION_COND_448_LE_8 ,
#define ARITH_OPERATION_COND_449_LE_8 ,
#define ARITH_OPERATION_COND_450_LE_8 ,
#define ARITH_OPERATION_COND_451_LE_8 ,
#define ARITH_OPERATION_COND_452_LE_8 ,
#define ARITH_OPERATION_COND_453_LE_8 ,
#define ARITH_OPERATION_COND_454_LE_8 ,
#define ARITH_OPERATION_COND_455_LE_8 ,
#define ARITH_OPERATION_COND_456_LE_8 ,
#define ARITH_OPERATION_COND_457_LE_8 ,
#define ARITH_OPERATION_COND_458_LE_8 ,
#define ARITH_OPERATION_COND_459_LE_8 ,
#define ARITH_OPERATION_COND_460_LE_8 ,
#define ARITH_OPERATION_COND_461_LE_8 ,
#define ARITH_OPERATION_COND_462_LE_8 ,
#define ARITH_OPERATION_COND_463_LE_8 ,
#define ARITH_OPERATION_COND_464_LE_8 ,
#define ARITH_OPERATION_COND_465_LE_8 ,
#define ARITH_OPERATION_COND_466_LE_8 ,
#define ARITH_OPERATION_COND_467_LE_8 ,
#define ARITH_OPERATION_COND_468_LE_8 ,
#define ARITH_OPERATION_COND_469_LE_8 ,
#define ARITH_OPERATION_COND_470_LE_8 ,
#define ARITH_OPERATION_COND_471_LE_8 ,
#define ARITH_OPERATION_COND_472_LE_8 ,
#define ARITH_OPERATION_COND_473_LE_8 ,
#define ARITH_OPERATION_COND_474_LE_8 ,
#define ARITH_OPERATION_COND_475_LE_8 ,
#define ARITH_OPERATION_COND_476_LE_8 ,
#define ARITH_OPERATION_COND_477_LE_8 ,
#define ARITH_OPERATION_COND_478_LE_8 ,
#define ARITH_OPERATION_COND_479_LE_8 ,
#define ARITH_OPERATION_COND_480_LE_8 ,
#define ARITH_OPERATION_COND_481_LE_8 ,
#define ARITH_OPERATION_COND_482_LE_8 ,
#define ARITH_OPERATION_COND_483_LE_8 ,
#define ARITH_OPERATION_COND_484_LE_8 ,
#define ARITH_OPERATION_COND_485_LE_8 ,
#define ARITH_OPERATION_COND_486_LE_8 ,
#define ARITH_OPERATION_COND_487_LE_8 ,
#define ARITH_OPERATION_COND_488_LE_8 ,
#define ARITH_OPERATION_COND_489_LE_8 ,
#define ARITH_OPERATION_COND_490_LE_8 ,
#define ARITH_OPERATION_COND_491_LE_8 ,
#define ARITH_OPERATION_COND_492_LE_8 ,
#define ARITH_OPERATION_COND_493_LE_8 ,
#define ARITH_OPERATION_COND_494_LE_8 ,
#define ARITH_OPERATION_COND_495_LE_8 ,
#define ARITH_OPERATION_COND_496_LE_8 ,
#define ARITH_OPERATION_COND_497_LE_8 ,
#define ARITH_OPERATION_COND_498_LE_8 ,
#define ARITH_OPERATION_COND_499_LE_8 ,


#endif /* _IEC_STD_FUNCTIONS_H */
