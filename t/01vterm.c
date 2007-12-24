#include "CUnit/CUnit.h"

#include "vterm.h"

vterm_t *e48;

int vterm_init(void)
{
  e48 = vterm_new(80, 25);
  return e48 ? 0 : 1;
}

static void test_getsize(void)
{
  int rows, cols;
  vterm_get_size(e48, &rows, &cols);

  CU_ASSERT_EQUAL(rows, 80);
  CU_ASSERT_EQUAL(cols, 25);
}

static void test_setsize(void)
{
  vterm_set_size(e48, 100, 40);

  int rows, cols;
  vterm_get_size(e48, &rows, &cols);

  CU_ASSERT_EQUAL(rows, 100);
  CU_ASSERT_EQUAL(cols, 40);
}


int vterm_fini(void)
{
  // TODO: free
  return 0;
}

#include "01vterm.inc"
