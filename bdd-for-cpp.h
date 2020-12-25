#include <stdio.h>
#include <vector>
#include <string>

#include <string.h>

using std::vector;
using std::string;

#include <stdarg.h>

struct __Bdd_Check__ {
    bool condition;
    int  line_number;
};

struct __Bdd_Single_Global_Context__ {
    // So we can make the output look pretty widepeepoHappy
    char const *__color_red__   = "\033[31m";
    char const *__color_blue__  = "\033[34m";
    char const *__color_reset__ = "\033[0m";
    char const *__current_color__;

    vector<__Bdd_Check__> __checks__;
    vector<__Bdd_Check__> __failed_checks__;

    char const     *__current_what__;
    vector<string>  __list_of_whats__;

    bool __run_once__ = true;
};

static __Bdd_Single_Global_Context__ __bdd_gc__;

// #define NO_BDD_COLOR_OUTPUT 1 // Color output can be disabled if need. Useful if a terminal like output program doesn't display ansi color.

static void __printf__(char const *format, ...) { // @Refactor: Setting the current color then printing doesn't seem all that great
#ifndef NO_BDD_COLOR_OUTPUT
    if (__bdd_gc__.__current_color__) {
        printf("%s", __bdd_gc__.__current_color__);
    }
#endif
    
    va_list args;
    va_start(args, format);
    
    vprintf(format, args);
    
    va_end(args);
    
#ifndef NO_BDD_COLOR_OUTPUT
    printf("%s", __bdd_gc__.__color_reset__);
    __bdd_gc__.__current_color__ = nullptr;
#endif
}

static void __bdd_function__(std::string const &, int *);
extern bool __bdd_function_is_defined__;

extern char const *__spec_name__;

int main() {
    if (__bdd_function_is_defined__ == false) {
        __bdd_gc__.__current_color__ = __bdd_gc__.__color_red__;
        __printf__("Error: ");
        __printf__("No specification block has been defined in this file!\n");
        return 1;
    }
    
    int test_count = 0;
    __bdd_function__("", &test_count); // First do a preliminary pass to see how many tests there are
    
    if (test_count == 0) {
        __bdd_gc__.__current_color__ = __bdd_gc__.__color_blue__;
        __printf__("Warning: ");
        __printf__("No it() blocks created\n");
        return 1;
    }
    
    __bdd_gc__.__current_color__ = __bdd_gc__.__color_blue__;
    __printf__("%s\n", __spec_name__);
    
    // Make sure everything is lined up properly
    size_t justify_amount = 0;
    for (auto const &what : __bdd_gc__.__list_of_whats__) {
        if (what.size() > justify_amount) {
            justify_amount = what.size();
        }
    }
    
    __bdd_gc__.__failed_checks__.clear();
    
    int i = 0;
    while (i < test_count) { // Now run each test
        __bdd_gc__.__checks__.clear();
        __bdd_function__(__bdd_gc__.__list_of_whats__[i], nullptr);
        
        if (__bdd_gc__.__checks__.size() == 0) {
            __bdd_gc__.__current_color__ = __bdd_gc__.__color_blue__;
            __printf__("Warning: ");
            __printf__("Zero checks within '%s'\n", __bdd_gc__.__current_what__);
            return 1;
        }
        
        size_t checks_passed = 0;
        for (auto const &check : __bdd_gc__.__checks__) {
            if (check.condition) {
                checks_passed++;
            } else {
                __bdd_gc__.__failed_checks__.push_back(check);
            }
        }
        
        // Finally print the score
        bool passed = checks_passed == __bdd_gc__.__checks__.size();
        __printf__("\t%-*s ", justify_amount, __bdd_gc__.__current_what__);
        __printf__("[%d/%d] %s\n", checks_passed, __bdd_gc__.__checks__.size(), passed ? "(PASSED)" : "(FAILED)");
        
        i++;
    }
    
    // Print out any failed checks
    if (__bdd_gc__.__failed_checks__.size() > 0) {
        printf("\n");
        for (auto const &check : __bdd_gc__.__failed_checks__) {
            printf("\tFailed check() at line %d\n", check.line_number);
        }
    }
}

#define spec(name) char const *__spec_name__ = name;                    \
bool __bdd_function_is_defined__ = true;                                \
void __bdd_function__(std::string const &__what__, int *__test_count__) \

#define run_once() for (; __bdd_gc__.__run_once__; __bdd_gc__.__run_once__ = false)

#define it(what)                                                                           \
if (__test_count__) { (*__test_count__)++; __bdd_gc__.__list_of_whats__.push_back(what); } \
for (bool __run__ = (strcmp(what, __what__.c_str()) == 0);                                 \
__run__ &&                                                                                 \
(__run__ ? __bdd_gc__.__current_what__ = what : 0);                                                   \
__run__ = false)                                                                           \

static bool __bdd_condition__;
static bool __push_check__(bool condition, int line_number) {
    __bdd_gc__.__checks__.push_back({ condition, line_number });
    return __bdd_condition__;
}


#define check(condition) (__bdd_condition__ = condition, __push_check__(__bdd_condition__, __LINE__))
