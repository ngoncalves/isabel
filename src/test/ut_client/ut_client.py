# -*- coding: iso-8859-15 -*-
"""
   Isabel: Qt Test Automation
   ==========================
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

   Unit tests for the client code
"""

import sys
import unittest
import random

# add the path to the client
sys.path.append('./client')
import client

class UT_SLIP(unittest.TestCase):
	"""
	Unit Tests for the SLIP encoding and decoding
	"""

	def test_slip_encode_decode_void(self):
		"""
		Test the SLIP encoding and decoding of a void array.
		"""
		print " - SLIP encoding/decoding an empty array"

		# encode an empty array
		slip = client.SLIP()
		empty = bytearray() 
		empty_enc = slip.encode(empty)
		self.assertEqual(2,len(empty_enc))
		self.assertEqual(slip.SLIP_END,empty_enc[0])
		self.assertEqual(slip.SLIP_END,empty_enc[1])

		# decode the result
		empty_dec = slip.decode(empty_enc)
		self.assertEqual(empty_dec,empty)		

	def test_slip_encode_decode_special(self):
		"""
		Test the SLIP encoding and decoding of an array with 
		the SLIP special chars.
		"""
		print " - SLIP encoding/decoding an array with special chars"
		
		# encode an array containing only the SLIP special chars
		slip = client.SLIP()
		special = bytearray([slip.SLIP_END,slip.SLIP_ESC]) 
		special_enc = slip.encode(special)
		self.assertEqual(6,len(special_enc))
		self.assertEqual(slip.SLIP_END,special_enc[0])
		self.assertEqual(slip.SLIP_ESC,special_enc[1])
		self.assertEqual(slip.SLIP_ESC_END,special_enc[2])
		self.assertEqual(slip.SLIP_ESC,special_enc[3])
		self.assertEqual(slip.SLIP_ESC_ESC,special_enc[4])
		self.assertEqual(slip.SLIP_END,special_enc[5])

		# decode the result
		special_dec = slip.decode(special_enc)
		self.assertEqual(special_dec,special)		

	def test_slip_encode_decode_arbitrary(self):
		"""
		Test the SLIP encoding/decoding of an arbitrary array.
		"""
		print " - SLIP encoding/decoding an arbitrary array"
		
		# encode an arbitrary array
		slip = client.SLIP()
		arbitrary = bytearray() 
		for b in range(0,50*1024):
			arbitrary.append(random.choice(range(0,256)))

		arbitrary_enc = slip.encode(arbitrary)
		self.assertTrue(len(arbitrary) + 2 <= len(arbitrary_enc))

		# decode the result
		arbitrary_dec = slip.decode(arbitrary_enc)
		self.assertEqual(arbitrary_dec,arbitrary)

if __name__ == '__main__':
	unittest.main()
