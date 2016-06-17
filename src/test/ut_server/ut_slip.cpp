/*
   Isabel
   =========
   Copyright (C) 2016  Nelson Gonçalves
   
   License
   -------

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Summary
   -------

   See the respective header file for details.
*/

#include "ut_slip.h"
#include "isabelSLIP.h"

#include <cassert>
#include <QByteArray>
#include <iostream>
#include <cstdlib>
#include <ctime>

/*-------------------- Test Cases Declaration -------------------------- */
/* Encode and decode an empty array.
*/
static void slip_encdec_void(void);

/* Encode and decode an array containing the SLIP special bytes.
*/
static void slip_encdec_special(void);

/* Encode and decode an arbitrary array.
*/
static void slip_encdec_arbitrary(void);

/*-------------------- Test Cases Main -------------------------- */
int ut_slip(void)
{
	std::cerr << "---------------------------" << std::endl; 
	std::cerr << "SLIP encoding and decoding " << std::endl; 
	std::cerr << "---------------------------" << std::endl; 

	/* run all of the test cases */
	slip_encdec_void();
	slip_encdec_special();
	slip_encdec_arbitrary();

	return 0; 
}

/*-------------------- Test Cases Implementation ---------------------- */

static void slip_encdec_void(void)
{
	std::cerr << " - encoding/decoding an empty array: "; 

	/* encode an empty array */
	QByteArray empty; 
	assert(0 == empty.size()); 

	QByteArray empty_enc = slip_encode(empty);
	assert(2 == empty_enc.size()); 
	assert(empty_enc[0] == empty_enc[1]); 
	assert(SLIP_END == (unsigned char)empty_enc[0]);

	/* decode the empty_enc array */
	empty = slip_decode(empty_enc);
	assert(0 == empty.size()); 

	std::cerr << "PASS" << std::endl; 
}

static void slip_encdec_special(void)
{
	std::cerr << " - encoding/decoding an array with SLIP chars: "; 

	/* encode an array containing only SLIP chars*/
	QByteArray special;
	special.append(SLIP_END);
	special.append(SLIP_ESC);
	assert(2 == special.size()); 

	QByteArray special_enc = slip_encode(special);
	assert(6 == special_enc.size()); 
	assert(SLIP_END     == (unsigned char)special_enc[0]);
	assert(SLIP_ESC     == (unsigned char)special_enc[1]);
	assert(SLIP_ESC_END == (unsigned char)special_enc[2]);
	assert(SLIP_ESC     == (unsigned char)special_enc[3]);
	assert(SLIP_ESC_ESC == (unsigned char)special_enc[4]);
	assert(SLIP_END     == (unsigned char)special_enc[5]);

	/* decode the special_enc array */
	special = slip_decode(special_enc);
	assert(2 == special.size()); 
	assert(SLIP_END == (unsigned char)special_enc[0]);
	assert(SLIP_ESC == (unsigned char)special_enc[1]);

	std::cerr << "PASS" << std::endl; 
}

static void slip_encdec_arbitrary(void)
{
	std::cerr << " - encoding/decoding an arbitrary array: "; 

	/* encode an array with random chars */
	int length = 50*1204; 

	QByteArray arbitrary(length,0x00);
	assert(length == arbitrary.size()); 

	srand(time(NULL));

	for(int i = 0; i < arbitrary.size(); i++)
	{
		arbitrary[i] = rand() % 256;
 	}

	QByteArray arbitrary_enc = slip_encode(arbitrary);
	assert((length + 2) <= arbitrary_enc.size()); 
	assert(SLIP_END == (unsigned char)arbitrary_enc[0]);
	assert(SLIP_END == (unsigned char)arbitrary_enc[arbitrary_enc.size() - 1]);
	
	/* decode the arbitrary_enc array */
	QByteArray arbitrary_dec = slip_decode(arbitrary_enc);
	assert(arbitrary_dec.size() == arbitrary.size()); 
	for(int i = 0; i < arbitrary.size(); i++)
	{
		assert(arbitrary_dec[i] == arbitrary[i]); 
 	}

	std::cerr << "PASS" << std::endl; 
}
