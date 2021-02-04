#define CATCH_CONFIG_MAIN

#include "third-party/Catch/single_include/catch2/catch.hpp"

#include "emp/bits/BitVector.hpp"
#include "emp/math/Random.hpp"

#include <sstream>
#include <map>
#include <limits>
#include <ratio>

#include "emp/base/vector.hpp"

TEST_CASE("1: Test BitVector Constructors", "[bits]"){
  // Create a size 50 bit vector, default to all zeros.
  emp::BitVector bv1(50);
  REQUIRE( bv1.GetSize() == 50 );
  REQUIRE( bv1.CountOnes() == 0 );
  REQUIRE( (~bv1).CountOnes() == 50 );

  // Create a size 1000 BitVector, default to all ones.
  emp::BitVector bv2(1000, true);
  REQUIRE( bv2.GetSize() == 1000 );
  REQUIRE( bv2.CountOnes() == 1000 );

  // Try a range of BitVector sizes, from 0 to 200.
  for (size_t bv_size = 0; bv_size <= 200; bv_size++) {
    emp::BitVector bv3(bv_size);
    REQUIRE( bv3.GetSize() == bv_size );
    REQUIRE( bv3.CountOnes() == 0 );
    for (size_t i = 0; i < bv_size; i++) bv3[i] = true;
    REQUIRE( bv3.CountOnes() == bv_size );
  }

  // Build a relatively large BitVector.
  emp::BitVector bv4(1000000);
  for (size_t i = 0; i < bv4.GetSize(); i += 100) bv4[i].Toggle();
  REQUIRE( bv4.CountOnes() == 10000 );

  // Try out the copy constructor.
  emp::BitVector bv5(bv4);
  REQUIRE( bv5.GetSize() == 1000000 );
  REQUIRE( bv5.CountOnes() == 10000 );

  // And the move constructor.
  auto old_ptr = bv5.RawBytes();         // Grab a pointer to where bv5 currently has its bytes.
  emp::BitVector bv6( std::move(bv5) );  // Move bv5 bytes into bv6.
  REQUIRE( bv6.RawBytes() == old_ptr );
  REQUIRE( bv5.RawBytes() == nullptr );

  // Construct from std::bitset.
  std::bitset<6> bit_set;
  bit_set[1] = 1;   bit_set[2] = 1;   bit_set[4] = 1;
  emp::BitVector bv7(bit_set);
  REQUIRE( bv7.GetSize() == 6 );
  REQUIRE( bv7.CountOnes() == 3 );

  // Construct from string.
  std::string bit_string = "10011001010000011101";
  emp::BitVector bv8(bit_string);
  REQUIRE( bv8.GetSize() == 20 );
  REQUIRE( bv8.CountOnes() == 9 );

  // Some random BitVectors
  emp::Random random;
  emp::BitVector bv9(1000, random);            // 50/50 chance for each bit.
  const size_t bv9_ones = bv9.CountOnes();
  REQUIRE( bv9_ones >= 400 );
  REQUIRE( bv9_ones <= 600 );

  emp::BitVector bv10(1000, random, 0.8);      // 80% chance of ones.
  const size_t bv10_ones = bv10.CountOnes();
  REQUIRE( bv10_ones >= 750 );
  REQUIRE( bv10_ones <= 850 );

  emp::BitVector bv11(1000, random, 117);      // Exactly 117 ones, randomly placed.
  const size_t bv11_ones = bv11.CountOnes();
  REQUIRE( bv11_ones == 117 );

  emp::BitVector bv12(bv11, 500);              // Construct with just first half of bv11.
  const size_t bv12_ones = bv12.CountOnes();
  REQUIRE( bv12_ones >= 20 );
  REQUIRE( bv12_ones <= 90 );

  emp::BitVector bv13({1,0,0,0,1,1,1,0,0,0,1,1,1}); // Construct with initializer list.
  REQUIRE( bv13.GetSize() == 13 );
  REQUIRE( bv13.CountOnes() == 7 );
}

TEST_CASE("2: Test BitVector Assignemnts", "[bits]"){
  emp::vector< emp::BitVector > v;

  // Try all BitVector sizes from 0 to 128.
  // Lot's of move operators will trigger as vector grows.
  for (size_t i = 0; i <= 128; i++) {
    v.emplace_back(i);
  }

  // And a few larger BitVectors...
  v.emplace_back(1023);
  v.emplace_back(1024);
  v.emplace_back(1025);
  v.emplace_back(1000000);

  // Copy each BitVector into bv2 and do some manipulations then copy back.
  for (emp::BitVector & bv : v) {
    emp::BitVector bv2 = bv;
    for (size_t i = 1; i < bv2.GetSize(); i += 2) {
      bv2[i] = 1;
    }
    bv = bv2;
  }

  // Now make sure the we constructed bits correctly!
  for (const emp::BitVector & bv : v) {
    REQUIRE( bv.CountOnes() == bv.GetSize()/2 );
  }

  emp::vector< emp::BitVector > v2;
  v2.push_back( emp::BitVector({0,1,0,1,0,1}) );

  v2 = v; // Copy over all BitVectors.

  std::bitset<600> bit_set;
  bit_set[1] = 1;   bit_set[22] = 1;   bit_set[444] = 1;

  v[10] = bit_set;  // Copy in an std::bitset.

  REQUIRE( v[10].GetSize() == 600 );
  REQUIRE( v[10].CountOnes() == 3 );

  std::string bit_string = "100110010100000111011001100101000001110110011001010000011101";

  v[75] = bit_string;

  REQUIRE( v[75].GetSize() == 60 );
  REQUIRE( v[75].CountOnes() == 27 );

}


TEST_CASE("3: Test BitVector Accessors", "[bits]"){
  emp::BitVector bv0(0);
  emp::BitVector bv1(1, true);
  emp::BitVector bv8( "10001101" );
  emp::BitVector bv32( "10001101100011011000110110001101" );
  emp::BitVector bv64( "1000110110001101100000011000110000001101100000000000110110001101" );
  emp::BitVector bv75( "010001011100010111110000011110100011111000001110100000111110010011111000011" );

  emp::Random random;
  emp::BitVector bv1k(1000, random, 0.75);
  
  // Make sure all sizes are correct.
  REQUIRE( bv0.GetSize() == 0 );
  REQUIRE( bv1.GetSize() == 1 );
  REQUIRE( bv8.GetSize() == 8 );
  REQUIRE( bv32.GetSize() == 32 );
  REQUIRE( bv64.GetSize() == 64 );
  REQUIRE( bv75.GetSize() == 75 );
  REQUIRE( bv1k.GetSize() == 1000 );

  // Check byte counts (should always round up!)
  REQUIRE( bv0.GetNumBytes() == 0 );
  REQUIRE( bv1.GetNumBytes() == 1 );     // round up!
  REQUIRE( bv8.GetNumBytes() == 1 );
  REQUIRE( bv32.GetNumBytes() == 4 );
  REQUIRE( bv64.GetNumBytes() == 8 );
  REQUIRE( bv75.GetNumBytes() == 10 );   // round up!
  REQUIRE( bv1k.GetNumBytes() == 125 );

  // How many states can be represented in each size of BitVector?
  REQUIRE( bv0.GetNumStates() == 1.0 );
  REQUIRE( bv1.GetNumStates() == 2.0 );
  REQUIRE( bv8.GetNumStates() == 256.0 );
  REQUIRE( bv32.GetNumStates() == 4294967296.0 );
  REQUIRE( bv64.GetNumStates() >= 18446744073709551610.0 );
  REQUIRE( bv64.GetNumStates() <= 18446744073709551720.0 );
  REQUIRE( bv75.GetNumStates() >= 37778931862957161709560.0 );
  REQUIRE( bv75.GetNumStates() <= 37778931862957161709570.0 );
  REQUIRE( bv1k.GetNumStates() == emp::Pow2(1000) );

  // Test Get()
  REQUIRE( bv1.Get(0) == 1 );
  REQUIRE( bv8.Get(0) == 1 );
  REQUIRE( bv8.Get(4) == 1 );
  REQUIRE( bv8.Get(6) == 0 );
  REQUIRE( bv8.Get(7) == 1 );
  REQUIRE( bv75.Get(0) == 0 );
  REQUIRE( bv75.Get(1) == 1 );
  REQUIRE( bv75.Get(72) == 0 );
  REQUIRE( bv75.Get(73) == 1 );
  REQUIRE( bv75.Get(74) == 1 );

  // Test Has() (including out of range)
  REQUIRE( bv0.Has(0) == false );
  REQUIRE( bv0.Has(1000000) == false );

  REQUIRE( bv1.Has(0) == true );
  REQUIRE( bv1.Has(1) == false );

  REQUIRE( bv8.Has(0) == true );
  REQUIRE( bv8.Has(4) == true );
  REQUIRE( bv8.Has(6) == false );
  REQUIRE( bv8.Has(7) == true );
  REQUIRE( bv8.Has(8) == false );

  REQUIRE( bv75.Has(0) == false );
  REQUIRE( bv75.Has(1) == true );
  REQUIRE( bv75.Has(72) == false );
  REQUIRE( bv75.Has(73) == true );
  REQUIRE( bv75.Has(74) == true );
  REQUIRE( bv75.Has(75) == false );
  REQUIRE( bv75.Has(79) == false );
  REQUIRE( bv75.Has(1000000) == false );

  // Test Set(), changing in most (but not all) cases.
  bv1.Set(0, 0);
  REQUIRE( bv1.Get(0) == 0 );
  bv8.Set(0, 1);
  REQUIRE( bv8.Get(0) == 1 );  // Already a 1!
  bv8.Set(4, 0);
  REQUIRE( bv8.Get(4) == 0 );
  bv8.Set(6, 1);
  REQUIRE( bv8.Get(6) == 1 );
  bv8.Set(7, 0);
  REQUIRE( bv8.Get(7) == 0 );
  bv75.Set(0, 1);
  REQUIRE( bv75.Get(0) == 1 );
  bv75.Set(1, 0);
  REQUIRE( bv75.Get(1) == 0 );
  bv75.Set(72, 1);
  REQUIRE( bv75.Get(72) == 1 );
  bv75.Set(73, 1);
  REQUIRE( bv75.Get(73) == 1 );  // Already a 1!
  bv75.Set(74, 0);
  REQUIRE( bv75.Get(74) == 0 );
}


TEST_CASE("Test BitVector", "[bits]")
{

	// Constructor
	emp::BitVector bv(10);

	// Get Size
	REQUIRE( (bv.GetSize() == 10) );
	REQUIRE( (bv.size() == 10) );

	// Set & Get
	bv.Set(0);
	REQUIRE(bv.Get(0));
	bv.Set(1, false);
	REQUIRE(!bv.Get(1));

	// Assignment operator
	emp::BitVector bv1(10);
	bv1 = bv;
	REQUIRE(bv1 == bv);
	REQUIRE(bv1.Get(0));
	emp::BitVector bv20(20);
	emp::BitVector bv30(30);
	bv20.Set(1);
	REQUIRE(bv20.Get(1));
	bv20 = bv;
	REQUIRE(bv20 == bv);
	REQUIRE(bv20.size()==bv.size());
	REQUIRE(!bv20.Get(1));
	bv20 = bv30;
	REQUIRE(!bv20.Get(1));
	REQUIRE(bv20 == bv30);

	// Resize
	bv1.Set(9);
	bv1.resize(8);
	REQUIRE( (bv1.GetSize() == 8) );
	REQUIRE( (bv1.GetByte(0) == 1) );
	bv1.resize(128);
	REQUIRE( (bv1.GetSize() == 128) );
	REQUIRE( (bv1.GetByte(1) == 0) );

	// Comparison operators
	REQUIRE((bv1 != bv));
	bv1.Resize(10);
	REQUIRE((bv1 == bv));
	REQUIRE((bv1 >= bv));
	REQUIRE((bv1 <= bv));
	bv.Set(1);
	REQUIRE((bv > bv1));
	REQUIRE((bv >= bv1));

	// Set & Get Byte
	emp::BitVector bv2(32);
	bv2.SetByte(0, 128);
	bv2.SetByte(1, 255);
	REQUIRE((bv2.GetByte(0) == 128));
	REQUIRE((bv2.GetByte(1) == 255));

	// Count Ones
	REQUIRE((bv2.CountOnes() == 9));
	REQUIRE((bv2.CountOnes_Sparse() == 9));
	REQUIRE((bv2.count() == 9));

	// Any All None SetAll Clear
	REQUIRE(bool(bv2)); // operator bool()
	REQUIRE(bool(bv2[7])); // bool operator[]
	REQUIRE(bv2.any());
	REQUIRE(!bv2.all());
	REQUIRE(!bv2.none());
	bv2.SetAll();
	REQUIRE(!bv2.none());
	REQUIRE(bv2.all());
	bv2.Clear();
	REQUIRE(bv2.none());
	REQUIRE(!bv2.all());

	// Prints
	std::stringstream ss;
	emp::BitVector bv3(8);
	bv3.SetByte(0,255);
	bv3.Print(ss);
	REQUIRE((ss.str() == "11111111"));
	ss.str(std::string()); // clear ss

	ss << bv3;
	REQUIRE((ss.str() == "11111111"));
	ss.str(std::string()); // clear ss

	bv3.SetByte(0,130);
	bv3.PrintOneIDs(ss);
	REQUIRE((ss.str() == "1 7 "));
	ss.str(std::string()); // clear ss

	bv3.PrintArray(ss);
	REQUIRE((ss.str() == "01000001"));
	ss.str(std::string()); // clear ss

	// Find & Pop Bit
	bv3.SetByte(0,74);
	REQUIRE((bv3.PopBit() == 1));
	REQUIRE((bv3.CountOnes() == 2));
	REQUIRE((bv3.GetByte(0) == 72));
	REQUIRE((bv3.FindBit() == 3));
	REQUIRE((bv3.FindBit(4) == 6));
	bv3.PopBit();
	bv3.PopBit();
	REQUIRE((bv3.FindBit() == -1));
	REQUIRE((bv3.FindBit(2) == -1));
	REQUIRE((bv3.PopBit() == -1));

	// Get Ones
	emp::vector<size_t> ones = bv3.GetOnes();
	REQUIRE((ones.size() == 0));
	bv3.SetByte(0,10);
	ones = bv3.GetOnes();
	REQUIRE((ones[0] == 1));
	REQUIRE((ones[1] == 3));

	// Larger BitVector
	emp::BitVector bv4(96);
	bv4.SetByte(1,1);
	bv4.PrintFields(ss);
	REQUIRE(ss.str() == "00000000000000000000000000000000 0000000000000000000000000000000000000000000000000000000100000000");

	// test single set.
	bv4[62] = 1;
	ss.str(std::string()); bv4.PrintFields(ss); // Clear & resend bits.
	REQUIRE(ss.str() == "00000000000000000000000000000000 0100000000000000000000000000000000000000000000000000000100000000");
	// test toggle of range (across boundary)
	bv4.Toggle(61, 70);
	ss.str(std::string()); bv4.PrintFields(ss); // Clear & resend bits.
	REQUIRE(ss.str() == "00000000000000000000000000111111 1010000000000000000000000000000000000000000000000000000100000000");
	// test clearing a range in a single field.
	bv4.Clear(65, 69);
	ss.str(std::string()); bv4.PrintFields(ss); // Clear & resend bits.
	REQUIRE(ss.str() == "00000000000000000000000000100001 1010000000000000000000000000000000000000000000000000000100000000");
	// test toggling a larger range
	bv4.Toggle(55, 75);
	ss.str(std::string()); bv4.PrintFields(ss); // Clear & resend bits.
	REQUIRE(ss.str() == "00000000000000000000011111011110 0101111110000000000000000000000000000000000000000000000100000000");
	// test clearing a field across bounderies
	bv4.Clear(56, 74);
	ss.str(std::string()); bv4.PrintFields(ss); // Clear & resend bits.
	REQUIRE(ss.str() == "00000000000000000000010000000000 0000000010000000000000000000000000000000000000000000000100000000");

	// Even longer bit vector (to test operations that span multiple fields)
	bv4.Resize(300);
	ss.str(std::string()); bv4.PrintFields(ss); // Clear & resend bits.
	REQUIRE(ss.str() == "00000000000000000000000000000000000000000000 0000000000000000000000000000000000000000000000000000000000000000 0000000000000000000000000000000000000000000000000000000000000000 0000000000000000000000000000000000000000000000000000010000000000 0000000010000000000000000000000000000000000000000000000100000000");
	// test setting a range that spans three fields.
	bv4.SetRange(100, 250);
	ss.str(std::string()); bv4.PrintFields(ss); // Clear & resend bits.
	REQUIRE(ss.str() == "00000000000000000000000000000000000000000000 0000001111111111111111111111111111111111111111111111111111111111 1111111111111111111111111111111111111111111111111111111111111111 1111111111111111111111111111000000000000000000000000010000000000 0000000010000000000000000000000000000000000000000000000100000000");
	// test clearing a full field.
	bv4.Clear(128,192);
	ss.str(std::string()); bv4.PrintFields(ss); // Clear & resend bits.
	REQUIRE(ss.str() == "00000000000000000000000000000000000000000000 0000001111111111111111111111111111111111111111111111111111111111 0000000000000000000000000000000000000000000000000000000000000000 1111111111111111111111111111000000000000000000000000010000000000 0000000010000000000000000000000000000000000000000000000100000000");
	// test clearing slightly more than a full field.
	bv4.Clear(127,193);
	ss.str(std::string()); bv4.PrintFields(ss); // Clear & resend bits.
	REQUIRE(ss.str() == "00000000000000000000000000000000000000000000 0000001111111111111111111111111111111111111111111111111111111110 0000000000000000000000000000000000000000000000000000000000000000 0111111111111111111111111111000000000000000000000000010000000000 0000000010000000000000000000000000000000000000000000000100000000");
	// test setting a full field.
	bv4.SetRange(128,192);
	ss.str(std::string()); bv4.PrintFields(ss); // Clear & resend bits.
	REQUIRE(ss.str() == "00000000000000000000000000000000000000000000 0000001111111111111111111111111111111111111111111111111111111110 1111111111111111111111111111111111111111111111111111111111111111 0111111111111111111111111111000000000000000000000000010000000000 0000000010000000000000000000000000000000000000000000000100000000");
	ss.str(std::string()); // clear ss


	// Logic operators
	emp::BitVector bv5(8);
	bv5.SetByte(0,28);
	REQUIRE((bv3.CountOnes() == 8-((~bv3).CountOnes())));
	REQUIRE(((bv3 & bv5).GetByte(0) == 8));
	REQUIRE(((bv3 | bv5).GetByte(0) == 30));
	REQUIRE(((bv3 ^ bv5).GetByte(0) == 22));
	REQUIRE(((bv3 << 2).GetByte(0) == 40));
	REQUIRE(((bv5 >> 2).GetByte(0) == 7));

	// Compound operators
	bv5 &= bv3;
	REQUIRE((bv5.GetByte(0) == 8));
	bv5 |= bv3;
	REQUIRE((bv5.GetByte(0) == 10));
	bv5 ^= bv3;
	REQUIRE((bv5.GetByte(0) == 0));
	bv3 >>= 2;
	REQUIRE((bv3.GetByte(0) == 2));
	bv3 <<= 4;
	REQUIRE((bv3.GetByte(0) == 32));

	// Hash
	emp::BitVector bv_a(2);
	bv_a.Set(0);
	emp::BitVector bv_b(2);
	bv_b.Set(0);
	REQUIRE(bv_a.Hash() == bv_b.Hash());
	bv_b.Set(0, false);
	REQUIRE(bv_a.Hash() != bv_b.Hash());
	bv_b.Set(0, true);

	// EQU_SELF
	REQUIRE(bv_a.EQU_SELF(bv_b).all());
	// bv_a = 01, bv_b = 01, ~(01 ^ 01) = 11
	REQUIRE(bv_a.GetByte(0) == 3);
	REQUIRE(bv_b.GetByte(0) == 1);
	REQUIRE(!(bv_a.EQU_SELF(bv_b).all()));
	// bv_a = 11, bv_b = 01, ~(11 ^ 01) = 01
	REQUIRE(bv_a.GetByte(0) == 1);
	REQUIRE(bv_b.GetByte(0) == 1);

	// NAND SELF
	// bv_a = 01, bv_b = 01, ~(01 & 01) = 10
	REQUIRE(bv_a.NAND_SELF(bv_b) == ~bv_b);
	REQUIRE(bv_a.GetByte(0) == 2);

	// NOR SELF
	// bv_a = 10, bv_b = 01, ~(10 | 01) = 00
	REQUIRE(bv_a.NOR_SELF(bv_b).none());
	REQUIRE(bv_a.GetByte(0) == 0);

	// NOT SELF
	REQUIRE(bv_a.NOT_SELF().all());

	// EQU
	emp::BitVector bv_c(3);
	bv_c.SetByte(0,2);
	emp::BitVector bv_d(3);
	bv_d.SetByte(0,2);
	REQUIRE(bv_c.EQU(bv_d).all());
	REQUIRE(bv_c.GetByte(0) == 2);

	// NAND
	REQUIRE(bv_c.NAND(bv_d) == ~bv_c);
	REQUIRE(bv_c.GetByte(0) == 2);

	// NOR
	REQUIRE(bv_c.NOR(bv_d) == ~bv_c);
	REQUIRE(bv_c.GetByte(0) == 2);

	// Bit proxy compound assignment operators
	// AND
	// bv_c = 010
	bv_c[0] &= 1;
	REQUIRE(bv_c[0] == 0);
	REQUIRE(bv_c[1] == 1);
	bv_c[1] &= 0;
	REQUIRE(bv_c[1] == 0);
	// OR
	// bv_d = 010
	bv_d[1] |= 0;
	REQUIRE(bv_d[1] == 1);
	bv_d[0] |= 1;
	REQUIRE(bv_d[0] == 1);
	bv_d[2] |= 0;
	REQUIRE(bv_d[2] == 0);
	// XOR
	// bv_c = 000
	bv_c[0] ^= 1;
	REQUIRE(bv_c[0] == 1);
	bv_c[0] ^= 1;
	REQUIRE(bv_c[0] == 0);
	//PLUS
	// bv_d = 011
	bv_d[2] += 1;
	REQUIRE(bv_d[2] == 1);
	// MINUS
	// bv_d = 111
	bv_d[1] -= 1;
	REQUIRE(bv_d[1] == 0);
	// TIMES
	//bv_d = 101
	bv_d[2] *= 1;
	REQUIRE(bv_d[2] == 1);
	bv_d[0] *= 0;
	REQUIRE(bv_d[0] == 0);
	// DIV
	// bv_c = 000
	bv_c[0] /= 1;
	REQUIRE(bv_c[0] == 0);

	// GetUInt SetUInt
	emp::BitVector bv_e(5);
	bv_e.SetUInt(0, 16);
	REQUIRE(bv_e.GetUInt(0) == 16);

	// Shift Left
	emp::BitVector bv_f(128);
	bv_f.SetAll();
	REQUIRE(bv_f.all());
	bv_f <<= 127;
	REQUIRE(bv_f.count() == 1);
	bv_f <<= 1;
	REQUIRE(bv_f.none());

	// #ifdef TDEBUG
	// REQUIRE(emp::assert_last_fail == 0);
	// #endif

}

TEST_CASE("Test MaskHigh, MaskLow", "[bits]") {

	// #ifdef TDEBUG
	// REQUIRE(emp::assert_last_fail == 0);
	// #endif

	// Test MaskHigh, MaskLow
	emp::BitVector a(0);
	a.Insert(0,true, 7);
	REQUIRE(a.Get(0));
	REQUIRE(a.Get(1));
	REQUIRE(a.Get(2));

	// #ifdef TDEBUG
	// REQUIRE(emp::assert_last_fail == 0);
	// #endif

}

TEST_CASE("Test PopBack, PushBack, Insert, Delete", "[bits]") {

	// #ifdef TDEBUG
	// REQUIRE(emp::assert_last_fail == 0);
	// #endif

	// Pop Back and Push Back
	emp::BitVector bv_g(0);
	bv_g.PushBack(true);
	bv_g.PushBack(true);
	bv_g.PushBack(false);
	REQUIRE(bv_g.Get(0));
	REQUIRE(bv_g.Get(1));
	REQUIRE(!bv_g.PopBack());
	REQUIRE(bv_g.size() == 2);

	// Insert and Delete
	bv_g.Insert(1, true);
	REQUIRE(bv_g.Get(0));
	REQUIRE(bv_g.Get(1));
	REQUIRE(bv_g.Get(2));
	REQUIRE(bv_g.size() == 3);

	bv_g.Insert(1, true);
	REQUIRE(bv_g.Get(3));
	REQUIRE(bv_g.Get(2));
	REQUIRE(bv_g.Get(1));
	REQUIRE(bv_g.Get(0));
	REQUIRE(bv_g.size() == 4);

	bv_g.Insert(1, false);
	REQUIRE(bv_g.Get(0));
	REQUIRE(!bv_g.Get(1));
	REQUIRE(bv_g.Get(2));
	REQUIRE(bv_g.Get(3));

	bv_g.Delete(0);
	REQUIRE(bv_g.size() == 4);
	REQUIRE(!bv_g.Get(0));
	bv_g.Delete(1, 2);
	REQUIRE(bv_g.size() == 2);
	REQUIRE(bv_g.Get(1));

	// #ifdef TDEBUG
	// REQUIRE(emp::assert_last_fail == 0);
	// #endif

}

TEST_CASE("Another Test BitVector", "[bits]") {

	// #ifdef TDEBUG
	// REQUIRE(emp::assert_last_fail == 0);
	// #endif

  emp::BitVector bv10(10);
  emp::BitVector bv32(32);
  emp::BitVector bv50(50);
  emp::BitVector bv64(64);
  emp::BitVector bv80(80);

  bv80[70] = 1;
  emp::BitVector bv80c(bv80);

  bv80 <<= 1;

  for (size_t i = 0; i < 75; i += 2) {
    emp::BitVector shift_vector = bv80 >> i;
    REQUIRE((shift_vector.CountOnes() == 1) == (i <= 71));
  }

  bv10 = (bv80 >> 70);

  // Test arbitrary bit retrieval of UInts
  bv80[65] = 1;
  REQUIRE(bv80.GetUInt32(2) == 130);
  REQUIRE(bv80.GetUIntAtBit(64) == 130);
//  REQUIRE(bv80.GetValueAtBit<5>(64) == 2);

	// #ifdef TDEBUG
	// REQUIRE(emp::assert_last_fail == 0);
	// #endif

}

TEST_CASE("Test range of BitVector constructors.", "[bits]")
// test list initializer
{
  emp::BitVector bs_empty{0,0,0};
  emp::BitVector bs_first{1,0,0};
  emp::BitVector bs_last{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
  emp::BitVector bs_two{0,0,1,0,0,0,0,0,0,0,1,0,0};
  emp::BitVector bs_full{1,1,1,1,1,1,1,1};

  REQUIRE(bs_empty.CountOnes() == 0);
  REQUIRE(bs_first.CountOnes() == 1);
  REQUIRE(bs_last.CountOnes() == 1);
  REQUIRE(bs_two.CountOnes() == 2);
  REQUIRE(bs_full.CountOnes() == 8);

  REQUIRE(bs_empty.GetSize() == 3);
  REQUIRE(bs_first.GetSize() == 3);
  REQUIRE(bs_last.GetSize() == 25);
  REQUIRE(bs_two.GetSize() == 13);
  REQUIRE(bs_full.GetSize() == 8);
}

// TEST_CASE("BitVector padding bits protected", "[bits]") {
// #ifdef TDEBUG

// 	REQUIRE(emp::assert_last_fail == 0);

//   for (size_t i = 1; i < 32; ++i) {

//     emp::BitVector vec(i);
//     REQUIRE(emp::assert_last_fail == 0);
//     vec.SetUInt(0, std::numeric_limits<uint32_t>::max());
//     REQUIRE(emp::assert_last_fail);
//     emp::assert_clear();

//   }

//   REQUIRE(emp::assert_last_fail == 0);

//   emp::BitVector vec(32);
//   vec.SetUInt(0, std::numeric_limits<uint32_t>::max());

//   REQUIRE(emp::assert_last_fail == 0);

// #endif
// }

TEST_CASE("BitVector regression test for #277", "[bits]") {
	// #ifdef TDEBUG
	// REQUIRE(emp::assert_last_fail == 0);
	// #endif

  emp::BitVector vec1(4);
  emp::BitVector vec2(4);

  for (size_t i = 0; i < 4; ++i) REQUIRE(!vec1[i]);
  for (size_t i = 0; i < 4; ++i) REQUIRE(!vec2[i]);
  vec1.SetUInt(0, 15);
  vec2.SetUIntAtBit(0, 15);
  for (size_t i = 0; i < 4; ++i) REQUIRE(vec1[i]);
  for (size_t i = 0; i < 4; ++i) REQUIRE(vec2[i]);

	// #ifdef TDEBUG
	// REQUIRE(emp::assert_last_fail == 0);
	// #endif

}
