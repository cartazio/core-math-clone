static inline TYPE_UNDER_TEST random_under_test (void)
{
  /* sample in [-4,4] */
  return 8 *  ((TYPE_UNDER_TEST) rand() / (TYPE_UNDER_TEST) RAND_MAX) - 4;
}
