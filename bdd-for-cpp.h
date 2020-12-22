#include <stdio.h>
#include <vector>
#include <string>

#include <string.h>

using std::vector;
using std::string;

#include <stdarg.h>

/*

Bdd For C++ is a header only library for testing your own code with the least amount of hassle possible. That means not only should it be headache free to setup the libary (one of the main reasons this is a header only library) but should have everything that a programmer will want.

===========================User Manual===========================

For every module in your program/project that you want to test create a single C++ file and include bdd_for_cpp.h.
Next create a specification block with the macro spec() and give it a name. You will probably want the name to be the module you're testing.

Example: test.cpp

#include "bdd_for_cpp.h"

spec("example") {
}

Only one specification can be defined in each file otherwise you will get a compiler error.

*/

// So we can make the output look pretty widePeepoHappy
static char const *color_red   = "\033[31m";
static char const *color_green = "\033[32m";
static char const *color_blue  = "\033[34m";
static char const *color_reset = "\033[0m";

#define NO_BDD_COLOR_OUTPUT 1 // Color output can be disabled if need. Useful if a terminal like output program doesn't display ansi color.

static char const *__current_color__;
static void __printf__(char const *format, ...) { // @Refactor: Setting the current color then printing doesn't seem all that great
#ifndef NO_BDD_COLOR_OUTPUT
    if (__current_color__) {
        printf("%s", __current_color__);
    }
#endif
    
    va_list args;
    va_start(args, format);
    
    vprintf(format, args);
    
    va_end(args);
    
#ifndef NO_BDD_COLOR_OUTPUT
    printf("%s", color_reset);
    __current_color__ = nullptr;
#endif
}

static void __bdd_function__(std::string const &, int *);
extern bool __bdd_function_is_defined__;

struct __Bdd_Check__ {
    bool condition;
    int  line_number;
};

static vector<__Bdd_Check__> __checks__;
static vector<__Bdd_Check__> __failed_checks__;

extern char const     *__spec_name__;
static char const     *__current_what__;
static vector<string>  __list_of_whats__;

int main() {
    if (__bdd_function_is_defined__ == false) {
        __current_color__ = color_red;
        __printf__("Error: ");
        __printf__("No specification block has been defined in this file!\n");
        return 1;
    }
    
    int test_count = 0;
    __bdd_function__("", &test_count); // First do a preliminary pass to see how many tests there are
    
    if (test_count == 0) {
        __current_color__ = color_blue;
        __printf__("Warning: ");
        __printf__("No it() blocks created\n");
        return 1;
    }
    
    __current_color__ = color_blue;
    __printf__("%s\n", __spec_name__);
    
    // Make sure everything is lined up properly
    size_t justify_amount = 0;
    for (auto const &what : __list_of_whats__) {
        if (what.size() > justify_amount) {
            justify_amount = what.size();
        }
    }
    
    __failed_checks__.clear();
    
    int i = 0;
    while (i < test_count) { // Now run each test
        __checks__.clear();
        __bdd_function__(__list_of_whats__[i], nullptr);
        
        if (__checks__.size() == 0) {
            __current_color__ = color_blue;
            __printf__("Warning: ");
            __printf__("Zero checks within '%s'\n", __current_what__);
            return 1;
        }
        
        size_t checks_passed = 0;
        for (auto const &check : __checks__) {
            if (check.condition) {
                checks_passed++;
            } else {
                __failed_checks__.push_back(check);
            }
        }
        
        // Finally print the score
        bool passed = checks_passed == __checks__.size();
        __printf__("\t%-*s ", justify_amount, __current_what__);
        __printf__("[%d/%d] %s\n", checks_passed, __checks__.size(), passed ? "(PASSED)" : "(FAILED)");
        
        i++;
    }
    
    // Print out any failed checks
    if (__failed_checks__.size() > 0) {
        printf("\n");
        for (auto const &check : __failed_checks__) {
            printf("\tFailed check() at line %d\n", check.line_number);
        }
    }
}

#define spec(name) char const *__spec_name__ = name;                        \
bool __bdd_function_is_defined__ = true;                                \
void __bdd_function__(std::string const &__what__, int *__test_count__) \

#define it(what)                                                                    \
if (__test_count__) { (*__test_count__)++; __list_of_whats__.push_back(what); } \
for (bool __run__ = (strcmp(what, __what__.c_str()) == 0);                      \
__run__ &&                                                                 \
(__run__ ? __current_what__ = what : 0);                                   \
__run__ = false)                                                           \

#define check(condition) __checks__.push_back({ condition, __LINE__ })