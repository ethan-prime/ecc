// precedence definitions for expr parsing
#define PREC_MULTIPLY 13
#define PREC_DIVIDE 13
#define PREC_REMAINDER 13

#define PREC_ADD 12
#define PREC_SUBTRACT 12

#define PREC_LEFT_SHIFT 11
#define PREC_RIGHT_SHIFT 11

#define PREC_LT 10
#define PREC_LTE 10
#define PREC_GT 10
#define PREC_GTE 10

#define PREC_LOGICAL_EQUAL 9
#define PREC_LOGICAL_NOT_EQUAL 9

#define PREC_BITWISE_AND 8
#define PREC_BITWISE_XOR 7
#define PREC_BITWISE_OR 6
#define PREC_LOGICAL_AND 5
#define PREC_LOGICAL_OR 4