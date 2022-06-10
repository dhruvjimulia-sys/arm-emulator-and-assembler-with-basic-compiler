static uint32_t rotate_right(uint32_t n, unsigned int shift_amount) {
  if ((shift_amount &= sizeof(n) * 8 - 1) == 0)
    return n;
  return (n << shift_amount) | (n >> (sizeof(n) * 8 - shift_amount));
}

void set_bits_to(uint32_t *instr, uint32_t input, unsigned int starting_at) {
        *instr |= (input << starting_at);
}
