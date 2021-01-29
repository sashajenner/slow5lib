#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <limits.h>
#include "slow5.h"
#include "slow5_extra.h"
#include "misc.h"

static char assert_fail[256];

#define ASSERT(statement) \
if (!(statement)) { \
    sprintf(assert_fail, "line %d: assertion `%s' failed", __LINE__, #statement); \
    return EXIT_FAILURE; \
}

#define CMD(foo) {#foo, foo},

typedef struct command command_t;

struct command {
	const char *str;
	int (*exe)(void);
};

int ato_xintx_valid(void) {
    int err;
    char buf[256];

    ASSERT(ato_uint8("0", &err) == 0);
    ASSERT(err == 0);
    ASSERT(ato_uint8("1", &err) == 1);
    ASSERT(err == 0);
    ASSERT(ato_uint8("100", &err) == 100);
    ASSERT(err == 0);
    sprintf(buf, "%" PRIu32, UINT8_MAX);
    ASSERT(ato_uint8(buf, &err) == UINT8_MAX);
    ASSERT(err == 0);

    ASSERT(ato_uint32("0", &err) == 0);
    ASSERT(err == 0);
    ASSERT(ato_uint32("1", &err) == 1);
    ASSERT(err == 0);
    ASSERT(ato_uint32("100", &err) == 100);
    ASSERT(err == 0);
    ASSERT(ato_uint32("2000", &err) == 2000);
    ASSERT(err == 0);
    sprintf(buf, "%" PRIu32, UINT32_MAX);
    ASSERT(ato_uint32(buf, &err) == UINT32_MAX);
    ASSERT(err == 0);

    return EXIT_SUCCESS;
}

int path_get_slow5_fmt_test(void) {

    ASSERT(FORMAT_ASCII == path_get_slow5_fmt("test.slow5"));
    ASSERT(FORMAT_ASCII == path_get_slow5_fmt("hithere/test.slow5"));
    ASSERT(FORMAT_ASCII == path_get_slow5_fmt("testaskdj.slow5"));
    ASSERT(FORMAT_ASCII == path_get_slow5_fmt("blow5.slow5"));
    ASSERT(FORMAT_ASCII == path_get_slow5_fmt("slow5.slow5"));
    ASSERT(FORMAT_ASCII == path_get_slow5_fmt("hi...slow5.slow5"));
    ASSERT(FORMAT_ASCII == path_get_slow5_fmt("1234.slow5"));
    ASSERT(FORMAT_ASCII == path_get_slow5_fmt("myslow5.slow5"));

    ASSERT(FORMAT_BINARY == path_get_slow5_fmt("test.blow5"));
    ASSERT(FORMAT_BINARY == path_get_slow5_fmt("hithere///test.blow5"));
    ASSERT(FORMAT_BINARY == path_get_slow5_fmt("hithere///test.slow5/test.blow5"));
    ASSERT(FORMAT_BINARY == path_get_slow5_fmt("testaslkdjlaskjdfalsdifaslkfdj234.blow5"));
    ASSERT(FORMAT_BINARY == path_get_slow5_fmt("blow5.blow5"));
    ASSERT(FORMAT_BINARY == path_get_slow5_fmt("slow5.blow5"));
    ASSERT(FORMAT_BINARY == path_get_slow5_fmt("hi...blow5.blow5"));
    ASSERT(FORMAT_BINARY == path_get_slow5_fmt("1234.blow5"));
    ASSERT(FORMAT_BINARY == path_get_slow5_fmt("myblow5.blow5"));

    // TODO should this be true
    ASSERT(FORMAT_ASCII == path_get_slow5_fmt(".slow5"));
    ASSERT(FORMAT_BINARY == path_get_slow5_fmt(".blow5"));

    ASSERT(FORMAT_UNKNOWN == path_get_slow5_fmt("..."));
    ASSERT(FORMAT_UNKNOWN == path_get_slow5_fmt("slow5"));
    ASSERT(FORMAT_UNKNOWN == path_get_slow5_fmt("blow5"));
    ASSERT(FORMAT_UNKNOWN == path_get_slow5_fmt("blablabla"));
    ASSERT(FORMAT_UNKNOWN == path_get_slow5_fmt(""));
    ASSERT(FORMAT_UNKNOWN == path_get_slow5_fmt(NULL));

    return EXIT_SUCCESS;
}

int name_get_slow5_fmt_test(void) {

    ASSERT(FORMAT_ASCII == name_get_slow5_fmt("slow5"));
    ASSERT(FORMAT_BINARY == name_get_slow5_fmt("blow5"));

    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt(".slow5"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt(".blow5"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt(".blow5"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt("..."));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt("slow55"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt("blow55"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt("blablabla"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt(""));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt(NULL));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt("test.slow5"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt("hithere/test.slow5"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt("testaskdj.slow5"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt("blow5.slow5"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt("slow5.slow5"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt("hi...slow5.slow5"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt("1234.slow5"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt("myslow5.slow5"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt("test.blow5"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt("hithere///test.blow5"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt("hithere///test.slow5/test.blow5"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt("testaslkdjlaskjdfalsdifaslkfdj234.blow5"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt("blow5.blow5"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt("slow5.blow5"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt("hi...blow5.blow5"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt("1234.blow5"));
    ASSERT(FORMAT_UNKNOWN == name_get_slow5_fmt("myblow5.blow5"));

    return EXIT_SUCCESS;
}

int slow5_open_valid(void) {
    struct slow5_file *s5p = slow5_open("test/data/exp/one_fast5/exp_1.slow5", "r");
    ASSERT(s5p != NULL);
    ASSERT(slow5_close(s5p) == 0);

    return EXIT_SUCCESS;
}

int slow5_open_null(void) {
    // Pathname NULL
    struct slow5_file *s5p = slow5_open(NULL, "r");
    ASSERT(s5p == NULL);
    ASSERT(slow5_close(s5p) == EOF);

    // Mode NULL
    s5p = slow5_open("test/data/exp/one_fast5/exp_1.slow5", NULL);
    ASSERT(s5p == NULL);
    ASSERT(slow5_close(s5p) == EOF);

    // Both NULL
    s5p = slow5_open(NULL, NULL);
    ASSERT(s5p == NULL);
    ASSERT(slow5_close(s5p) == EOF);

    return EXIT_SUCCESS;
}

int slow5_open_invalid(void) {
    // Path invalid
    struct slow5_file *s5p = slow5_open("test/data/exp/one_fast5_invalid/exp_1.slow5", "r");
    ASSERT(s5p == NULL);
    ASSERT(slow5_close(s5p) == EOF);

    // Extension invalid
    s5p = slow5_open("test/data/err/one_fast5/exp_1.slow", "r");
    ASSERT(s5p == NULL);
    ASSERT(slow5_close(s5p) == EOF);

    return EXIT_SUCCESS;
}

int slow5_open_with_valid(void) {
    struct slow5_file *s5p = slow5_open_with("test/data/exp/one_fast5/exp_1.slow5", "r", FORMAT_ASCII);
    ASSERT(s5p != NULL);
    ASSERT(slow5_close(s5p) == 0);

    s5p = slow5_open_with("test/data/err/one_fast5/exp_1.slow", "r", FORMAT_ASCII);
    ASSERT(s5p != NULL);
    ASSERT(slow5_close(s5p) == 0);

    s5p = slow5_open_with("test/data/exp/one_fast5/exp_1.slow5", "r", FORMAT_UNKNOWN);
    ASSERT(s5p != NULL);
    ASSERT(slow5_close(s5p) == 0);

    return EXIT_SUCCESS;
}

int slow5_open_with_null(void) {
    // Pathname NULL
    struct slow5_file *s5p = slow5_open_with(NULL, "r", FORMAT_ASCII);
    ASSERT(s5p == NULL);
    ASSERT(slow5_close(s5p) == EOF);

    // Mode NULL
    s5p = slow5_open_with("test/data/exp/one_fast5/exp_1.slow5", NULL, FORMAT_BINARY);
    ASSERT(s5p == NULL);
    ASSERT(slow5_close(s5p) == EOF);

    // Both NULL
    s5p = slow5_open_with(NULL, NULL, FORMAT_UNKNOWN);
    ASSERT(s5p == NULL);
    ASSERT(slow5_close(s5p) == EOF);

    return EXIT_SUCCESS;
}

int slow5_open_with_invalid(void) {
    // Format invalid
    struct slow5_file *s5p = slow5_open_with("test/data/exp/one_fast5/exp_1.slow5", "r", FORMAT_BINARY);
    ASSERT(s5p == NULL);
    ASSERT(slow5_close(s5p) == EOF);

    // Extension invalid
    s5p = slow5_open_with("test/data/err/one_fast5/exp_1.slow", "r", FORMAT_UNKNOWN);
    ASSERT(s5p == NULL);
    ASSERT(slow5_close(s5p) == EOF);

    return EXIT_SUCCESS;
}

int slow5_get_valid(void) {
    struct slow5_file *s5p = slow5_open("test/data/exp/one_fast5/exp_1.slow5", "r");
    ASSERT(s5p != NULL);

    struct slow5_rec *read = NULL;
    ASSERT(slow5_get("a649a4ae-c43d-492a-b6a1-a5b8b8076be4", &read, s5p) == 0);
    slow5_rec_free(read);

    ASSERT(slow5_close(s5p) == 0);

    return EXIT_SUCCESS;
}

int slow5_get_null(void) {
    struct slow5_file *s5p = slow5_open("test/data/exp/one_fast5/exp_1.slow5", "r");
    ASSERT(s5p != NULL);

    struct slow5_rec *read = NULL;
    ASSERT(slow5_get(NULL, &read, s5p) == -1);
    ASSERT(slow5_get("a649a4ae-c43d-492a-b6a1-a5b8b8076be4", NULL, s5p) == -1);
    ASSERT(slow5_get("a649a4ae-c43d-492a-b6a1-a5b8b8076be4", &read, NULL) == -1);
    ASSERT(slow5_get(NULL, NULL, s5p) == -1);
    ASSERT(slow5_get(NULL, &read, NULL) == -1);
    ASSERT(slow5_get("a649a4ae-c43d-492a-b6a1-a5b8b8076be4", NULL, NULL) == -1);
    ASSERT(slow5_get(NULL, NULL, NULL) == -1);
    // Shouldn't need to slow5_rec_free

    ASSERT(slow5_close(s5p) == 0);

    return EXIT_SUCCESS;
}

int slow5_get_invalid(void) {
    struct slow5_file *s5p = slow5_open("test/data/exp/one_fast5/exp_1.slow5", "r");
    ASSERT(s5p != NULL);

    // TODO simulate -2 error

    struct slow5_rec *read = NULL;
    ASSERT(slow5_get("badreadid", &read, s5p) == -3);
    ASSERT(slow5_get("", &read, s5p) == -3);
    ASSERT(slow5_get("a649a4ae-c43d-492a-b6a1-a5b8b8076be", &read, s5p) == -3);
    ASSERT(slow5_get("O_O", &read, s5p) == -3);

    ASSERT(slow5_close(s5p) == 0);

    // TODO simulate -4 error

    s5p = slow5_open("test/data/err/parse_bad.slow5", "r");
    ASSERT(s5p != NULL);

    ASSERT(slow5_get("a649a4ae-c43d-492a-b6a1-a5b8b8076be4", &read, s5p) == -5);
    ASSERT(slow5_get("1", &read, s5p) == -5);
    ASSERT(slow5_get("2", &read, s5p) == -5);
    ASSERT(slow5_get("3", &read, s5p) == -5);
    ASSERT(slow5_get("4", &read, s5p) == -5);
    ASSERT(slow5_get("5", &read, s5p) == -5);
    ASSERT(slow5_get("6", &read, s5p) == -5);
    ASSERT(slow5_get("7", &read, s5p) == -5);
    slow5_rec_free(read);

    ASSERT(slow5_close(s5p) == 0);

    return EXIT_SUCCESS;
}

int slow5_get_many_same(void) {
    struct slow5_file *s5p = slow5_open("test/data/exp/one_fast5/exp_1.slow5", "r");
    ASSERT(s5p != NULL);

    struct slow5_rec *read = NULL;
    ASSERT(slow5_get("a649a4ae-c43d-492a-b6a1-a5b8b8076be4", &read, s5p) == 0);
    ASSERT(slow5_get("a649a4ae-c43d-492a-b6a1-a5b8b8076be4", &read, s5p) == 0);
    ASSERT(slow5_get("a649a4ae-c43d-492a-b6a1-a5b8b8076be4", &read, s5p) == 0);
    ASSERT(slow5_get("a649a4ae-c43d-492a-b6a1-a5b8b8076be4", &read, s5p) == 0);
    ASSERT(slow5_get("a649a4ae-c43d-492a-b6a1-a5b8b8076be4", &read, s5p) == 0);
    ASSERT(slow5_get("a649a4ae-c43d-492a-b6a1-a5b8b8076be4", &read, s5p) == 0);
    ASSERT(slow5_get("a649a4ae-c43d-492a-b6a1-a5b8b8076be4", &read, s5p) == 0);
    slow5_rec_free(read);

    ASSERT(slow5_close(s5p) == 0);

    return EXIT_SUCCESS;
}

int slow5_get_many_different(void) {
    struct slow5_file *s5p = slow5_open("test/data/test/same_diff_ids.slow5", "r");
    ASSERT(s5p != NULL);

    struct slow5_rec *read = NULL;
    ASSERT(slow5_get("1", &read, s5p) == 0);
    ASSERT(slow5_get("2", &read, s5p) == 0);
    ASSERT(slow5_get("3", &read, s5p) == 0);
    ASSERT(slow5_get("4", &read, s5p) == 0);
    ASSERT(slow5_get("5", &read, s5p) == 0);
    ASSERT(slow5_get("6", &read, s5p) == 0);
    ASSERT(slow5_get("7", &read, s5p) == 0);
    ASSERT(slow5_get("8", &read, s5p) == 0);
    ASSERT(slow5_get("9", &read, s5p) == 0);
    ASSERT(slow5_get("10", &read, s5p) == 0);
    ASSERT(slow5_get("--", &read, s5p) == 0);
    ASSERT(slow5_get("", &read, s5p) == 0);
    ASSERT(slow5_get(".", &read, s5p) == 0);
    ASSERT(slow5_get("^_^", &read, s5p) == 0);
    ASSERT(slow5_get("lol", &read, s5p) == 0);
    ASSERT(slow5_get("weirdid", &read, s5p) == 0);
    slow5_rec_free(read);

    ASSERT(slow5_close(s5p) == 0);

    return EXIT_SUCCESS;
}


int slow5_get_next_valid(void) {
    struct slow5_file *s5p = slow5_open("test/data/exp/one_fast5/exp_1.slow5", "r");
    ASSERT(s5p != NULL);

    struct slow5_rec *read = NULL;
    ASSERT(slow5_get_next(&read, s5p) == 0);
    slow5_rec_free(read);

    ASSERT(slow5_close(s5p) == 0);

    return EXIT_SUCCESS;
}

int slow5_get_next_null(void) {
    struct slow5_file *s5p = slow5_open("test/data/exp/one_fast5/exp_1.slow5", "r");
    ASSERT(s5p != NULL);

    struct slow5_rec *read = NULL;
    ASSERT(slow5_get_next(NULL, s5p) == -1);
    ASSERT(slow5_get_next(&read, NULL) == -1);
    ASSERT(slow5_get_next(NULL, NULL) == -1);
    slow5_rec_free(read);

    ASSERT(slow5_close(s5p) == 0);

    return EXIT_SUCCESS;
}

int slow5_get_next_empty(void) {
    struct slow5_file *s5p = slow5_open("test/data/exp/one_fast5/exp_1.slow5", "r");
    ASSERT(s5p != NULL);

    struct slow5_rec *read = NULL;
    ASSERT(slow5_get_next(&read, s5p) == 0);
    ASSERT(slow5_get_next(&read, s5p) == -2);
    ASSERT(slow5_get_next(&read, s5p) == -2);
    ASSERT(slow5_get_next(&read, s5p) == -2);
    ASSERT(slow5_get_next(&read, s5p) == -2);
    slow5_rec_free(read);

    ASSERT(slow5_close(s5p) == 0);

    return EXIT_SUCCESS;
}

int slow5_get_next_invalid(void) {
    struct slow5_file *s5p = slow5_open("test/data/err/parse_bad.slow5", "r");
    ASSERT(s5p != NULL);

    struct slow5_rec *read = NULL;
    ASSERT(slow5_get_next(&read, s5p) == -3);
    ASSERT(slow5_get_next(&read, s5p) == -3);
    ASSERT(slow5_get_next(&read, s5p) == -3);
    ASSERT(slow5_get_next(&read, s5p) == -3);
    ASSERT(slow5_get_next(&read, s5p) == -3);
    ASSERT(slow5_get_next(&read, s5p) == -3);
    ASSERT(slow5_get_next(&read, s5p) == -3);
    ASSERT(slow5_get_next(&read, s5p) == -3);
    slow5_rec_free(read);

    ASSERT(slow5_close(s5p) == 0);
    return EXIT_SUCCESS;
}


struct command tests[] = {
    CMD(ato_xintx_valid)

    CMD(path_get_slow5_fmt_test)
    CMD(name_get_slow5_fmt_test)

    CMD(slow5_open_valid)
    CMD(slow5_open_null)
    CMD(slow5_open_invalid)

    CMD(slow5_open_with_valid)
    CMD(slow5_open_with_null)
    //CMD(slow5_open_with_invalid)

    CMD(slow5_get_valid)
    CMD(slow5_get_null)
    //CMD(slow5_get_invalid)
    CMD(slow5_get_many_same)
    CMD(slow5_get_many_different)

    CMD(slow5_get_next_valid)
    CMD(slow5_get_next_null)
    CMD(slow5_get_next_empty)
    //CMD(slow5_get_next_invalid)
};

int main(void) {
    int test_n = sizeof (tests) / sizeof (struct command);
    int ret = EXIT_SUCCESS;
    int n_passed = 0;

    for (int i = 0; i < test_n; ++ i) {
        if (tests[i].exe() == EXIT_FAILURE) {
            fprintf(stderr, "%s\t\t%s\n", tests[i].str, assert_fail);
            ret = EXIT_FAILURE;
        } else {
            ++ n_passed;
        }
    }

    // Print number of tests passed
    fprintf(stderr, "%d/%d\n", n_passed, test_n);

    return ret;
}
