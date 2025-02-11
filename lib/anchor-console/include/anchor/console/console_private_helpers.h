#pragma once

#include "anchor/console/console_config.h"

#if !defined(__GNUC__) && !defined(__TI_COMPILER_VERSION__)
#error "Unsupported toolchain"
#endif

// The _CONSOLE_NUM_ARGS(...) macro returns the number of arguments passed to it (0-10)
#define _CONSOLE_NUM_ARGS(...) _CONSOLE_NUM_ARGS_HELPER1(_, ##__VA_ARGS__, _CONSOLE_NUM_ARGS_SEQ())
#define _CONSOLE_NUM_ARGS_HELPER1(...) _CONSOLE_NUM_ARGS_HELPER2(__VA_ARGS__)
#define _CONSOLE_NUM_ARGS_HELPER2(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define _CONSOLE_NUM_ARGS_SEQ() 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#ifndef __cplusplus
_Static_assert(_CONSOLE_NUM_ARGS() == 0, "_CONSOLE_NUM_ARGS() != 0");
#endif
_Static_assert(_CONSOLE_NUM_ARGS(1) == 1, "_CONSOLE_NUM_ARGS(1) != 1");
_Static_assert(_CONSOLE_NUM_ARGS(1, 2, 3, 4, 5, 6, 7, 8, 9, 10) == 10, "_CONSOLE_NUM_ARGS(<10_args>) != 10");

// General helper macro for concatenating two tokens
#define _CONSOLE_CONCAT(A, B) _CONSOLE_CONCAT2(A, B)
#define _CONSOLE_CONCAT2(A, B) A##B

// Helper macro for calling a given macro for each of 0-10 arguments
#define _CONSOLE_MAP(X, ...)                                                                                           \
	_CONSOLE_MAP_HELPER(_CONSOLE_CONCAT(_CONSOLE_MAP_, _CONSOLE_NUM_ARGS(__VA_ARGS__)), X, ##__VA_ARGS__)
#define _CONSOLE_MAP_HELPER(C, X, ...) C(X, ##__VA_ARGS__)
#define _CONSOLE_MAP_0(X)
#define _CONSOLE_MAP_1(X, _1) X(_1)
#define _CONSOLE_MAP_2(X, _1, _2) X(_1) X(_2)
#define _CONSOLE_MAP_3(X, _1, _2, _3) X(_1) X(_2) X(_3)
#define _CONSOLE_MAP_4(X, _1, _2, _3, _4) X(_1) X(_2) X(_3) X(_4)
#define _CONSOLE_MAP_5(X, _1, _2, _3, _4, _5) X(_1) X(_2) X(_3) X(_4) X(_5)
#define _CONSOLE_MAP_6(X, _1, _2, _3, _4, _5, _6) X(_1) X(_2) X(_3) X(_4) X(_5) X(_6)
#define _CONSOLE_MAP_7(X, _1, _2, _3, _4, _5, _6, _7) X(_1) X(_2) X(_3) X(_4) X(_5) X(_6) X(_7)
#define _CONSOLE_MAP_8(X, _1, _2, _3, _4, _5, _6, _7, _8) X(_1) X(_2) X(_3) X(_4) X(_5) X(_6) X(_7) X(_8)
#define _CONSOLE_MAP_9(X, _1, _2, _3, _4, _5, _6, _7, _8, _9) X(_1) X(_2) X(_3) X(_4) X(_5) X(_6) X(_7) X(_8) X(_9)
#define _CONSOLE_MAP_10(X, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10)                                                    \
	X(_1) X(_2) X(_3) X(_4) X(_5) X(_6) X(_7) X(_8) X(_9) X(_10)

// Helper macros for defining things differently if the command has args or not
#define _CONSOLE_IF_ARGS(VAL, ...) _CONSOLE_IF_ELSE_NO_ARGS(, VAL, ##__VA_ARGS__)
#define _CONSOLE_IF_NO_ARGS(VAL, ...) _CONSOLE_IF_ELSE_NO_ARGS(VAL, , ##__VA_ARGS__)
#define _CONSOLE_IF_ELSE_NO_ARGS(TRUE, FALSE, ...)                                                                     \
	_CONSOLE_IF_ELSE_NO_ARGS_HELPER(                                                                                   \
		_CONSOLE_CONCAT(_CONSOLE_IF_ELSE_NO_ARGS_, _CONSOLE_NUM_ARGS(__VA_ARGS__)), TRUE, FALSE, ##__VA_ARGS__)
#define _CONSOLE_IF_ELSE_NO_ARGS_HELPER(C, TRUE, FALSE, ...) C(TRUE, FALSE, ##__VA_ARGS__)
#define _CONSOLE_IF_ELSE_NO_ARGS_0(TRUE, FALSE) TRUE
#define _CONSOLE_IF_ELSE_NO_ARGS_1(TRUE, FALSE, ...) FALSE
#define _CONSOLE_IF_ELSE_NO_ARGS_2(TRUE, FALSE, ...) FALSE
#define _CONSOLE_IF_ELSE_NO_ARGS_3(TRUE, FALSE, ...) FALSE
#define _CONSOLE_IF_ELSE_NO_ARGS_4(TRUE, FALSE, ...) FALSE
#define _CONSOLE_IF_ELSE_NO_ARGS_5(TRUE, FALSE, ...) FALSE
#define _CONSOLE_IF_ELSE_NO_ARGS_6(TRUE, FALSE, ...) FALSE
#define _CONSOLE_IF_ELSE_NO_ARGS_7(TRUE, FALSE, ...) FALSE
#define _CONSOLE_IF_ELSE_NO_ARGS_8(TRUE, FALSE, ...) FALSE
#define _CONSOLE_IF_ELSE_NO_ARGS_9(TRUE, FALSE, ...) FALSE
#define _CONSOLE_IF_ELSE_NO_ARGS_10(TRUE, FALSE, ...) FALSE

// Helper macros for defining console commands and their arguments
#define _CONSOLE_COMMAND_DEF(CMD, DESC, ...)                                                                           \
	_CONSOLE_COMMAND_ARGS_AND_HANDLER(CMD, ##__VA_ARGS__)                                                              \
	_CONSOLE_COMMAND_DEF_TAB_COMPLETE_ITER_DEFAULT_DEF(CMD)                                                            \
	static const console_command_def_t _##CMD##_DEF = {                                                                \
		.name = #CMD,                                                                                                  \
		_CONSOLE_COMMAND_DEF_DESC_FIELD(DESC)                                                                          \
			_CONSOLE_IF_ELSE_NO_ARGS(.handler_no_args = (console_command_handler_no_args_t)CMD##_command_handler,      \
									 .handler = (console_command_handler_t)CMD##_command_handler,                      \
									 ##__VA_ARGS__),                                                                   \
		_CONSOLE_COMMAND_DEF_TAB_COMPLETE_ITER_FIELD(CMD).args = _##CMD##_ARGS_DEF,                                    \
		.num_args = sizeof(_##CMD##_ARGS_DEF) / sizeof(console_arg_def_t),                                             \
		.args_ptr = _##CMD##_ARGS,                                                                                     \
	};                                                                                                                 \
	static const console_command_def_t *const CMD = &_##CMD##_DEF
#define _CONSOLE_COMMAND_DEF_WITH_TAB_COMPLETION(CMD, DESC, ...)                                                       \
	_CONSOLE_COMMAND_ARGS_AND_HANDLER(CMD, ##__VA_ARGS__)                                                              \
	_CONSOLE_COMMAND_DEF_TAB_COMPLETE_ITER_PROTOTYPE(CMD)                                                              \
	static const console_command_def_t _##CMD##_DEF = {                                                                \
		.name = #CMD,                                                                                                  \
		_CONSOLE_COMMAND_DEF_DESC_FIELD(DESC)                                                                          \
			_CONSOLE_IF_ELSE_NO_ARGS(.handler_no_args = (console_command_handler_no_args_t)CMD##_command_handler,      \
									 .handler = (console_command_handler_t)CMD##_command_handler,                      \
									 ##__VA_ARGS__),                                                                   \
		_CONSOLE_COMMAND_DEF_TAB_COMPLETE_ITER_FIELD(CMD).args = _##CMD##_ARGS_DEF,                                    \
		.num_args = sizeof(_##CMD##_ARGS_DEF) / sizeof(console_arg_def_t),                                             \
		.args_ptr = _##CMD##_ARGS,                                                                                     \
	};                                                                                                                 \
	static const console_command_def_t *const CMD = &_##CMD##_DEF
#if CONSOLE_HELP_COMMAND
#define _CONSOLE_COMMAND_DEF_DESC_FIELD(DESC) .desc = DESC,
#else
#define _CONSOLE_COMMAND_DEF_DESC_FIELD(DESC)
#endif
#if CONSOLE_TAB_COMPLETE
#define _CONSOLE_COMMAND_DEF_TAB_COMPLETE_ITER_DEFAULT_DEF(CMD)                                                        \
	static const console_tab_complete_iterator_t CMD##_tab_complete_iterator = NULL;
#define _CONSOLE_COMMAND_DEF_TAB_COMPLETE_ITER_PROTOTYPE(CMD)                                                          \
	static const char *CMD##_tab_complete_iterator(bool start);
#define _CONSOLE_COMMAND_DEF_TAB_COMPLETE_ITER_FIELD(CMD) .tab_complete_iter = CMD##_tab_complete_iterator,
#else
#define _CONSOLE_COMMAND_DEF_TAB_COMPLETE_ITER_DEFAULT_DEF(CMD)
#define _CONSOLE_COMMAND_DEF_TAB_COMPLETE_ITER_PROTOTYPE(CMD)
#define _CONSOLE_COMMAND_DEF_TAB_COMPLETE_ITER_FIELD(CMD)
#endif
#define _CONSOLE_COMMAND_ARGS_AND_HANDLER(CMD, ...)                                                                    \
	_CONSOLE_IF_ARGS(typedef struct {_CONSOLE_MAP(_CONSOLE_ARG_TYPE_HELPER, ##__VA_ARGS__)} CMD##_args_t;              \
					 _Static_assert(sizeof(CMD##_args_t) == _CONSOLE_NUM_ARGS(__VA_ARGS__) * sizeof(void *),           \
									"Compiler created *_args_t struct of unexpected size");                            \
					 , ##__VA_ARGS__)                                                                                  \
	static const console_arg_def_t _##CMD##_ARGS_DEF[] = {_CONSOLE_MAP(_CONSOLE_ARG_DEF_HELPER, ##__VA_ARGS__)};       \
	_Static_assert(sizeof(_##CMD##_ARGS_DEF) / sizeof(console_arg_def_t) == _CONSOLE_NUM_ARGS(__VA_ARGS__),            \
				   "Internal error in code generation");                                                               \
	static void *_##CMD##_ARGS[_CONSOLE_NUM_ARGS(__VA_ARGS__)];                                                        \
	static void CMD##_command_handler(_CONSOLE_IF_ELSE_NO_ARGS(void, const CMD##_args_t *args, ##__VA_ARGS__));
#define _CONSOLE_ARG_DEF_HELPER(...) _CONSOLE_ARG_DEF_HELPER2 __VA_ARGS__
#if CONSOLE_HELP_COMMAND
#define _CONSOLE_ARG_DEF_HELPER2(NAME, ENUM_TYPE, IS_OPTIONAL, C_TYPE, DESC)                                           \
	{.name = #NAME, .desc = DESC, .type = ENUM_TYPE, .is_optional = IS_OPTIONAL},
#else
#define _CONSOLE_ARG_DEF_HELPER2(NAME, ENUM_TYPE, IS_OPTIONAL, C_TYPE)                                                 \
	{.name = #NAME, .type = ENUM_TYPE, .is_optional = IS_OPTIONAL},
#endif
#define _CONSOLE_ARG_TYPE_HELPER(...) _CONSOLE_ARG_TYPE_HELPER2 __VA_ARGS__
#define _CONSOLE_ARG_TYPE_HELPER2(NAME, ENUM_TYPE, IS_OPTIONAL, C_TYPE, ...) C_TYPE NAME;
