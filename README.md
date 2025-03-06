# pos-dist

This project consists of testing for various proof of space distance algorithms.

# To Do

## Distance formulas
- [ ] Implement [Hamming Distance](https://en.wikipedia.org/wiki/Hamming_distance)
- [ ] Implement Chia distance algorithm

## Generation
- [ ] Generate nonce to hash table based off of k value
    - Can save these tables so we don't have to regenerate as shouldn't ever change
- [ ] Generate second table based off of the different distance formulas

## Statistics
- [ ] Total number of pairs within distance criteria vs number neighboring on 
    sorted table
    - Distance between pairs in this criteria

# Layout

`distance_algos.c` & `distance_algos.h`: contains all distance algorithm implementations

`table_gen.c` & `table_gen.h`: used to generate tables

`test.c`: brings all files together to run tests
