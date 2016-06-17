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
#include "isabelSLIP.h"

/*--------------------- Public Function Definitions ----------------*/

QByteArray slip_encode(const QByteArray &input)
{
	QByteArray output;

	output.append(SLIP_END);

	for(int i = 0; i < input.size(); i++)
	{
		switch((unsigned char)input.at(i))
		{
			case SLIP_END:
				output.append(SLIP_ESC);
				output.append(SLIP_ESC_END);
				break; 

			case SLIP_ESC:
				output.append(SLIP_ESC);
				output.append(SLIP_ESC_ESC);
				break; 

			default:
				output.append(input.at(i));
				break; 
		}
	}

	output.append(SLIP_END);

	return output;
}

QByteArray slip_decode(const QByteArray &input)
{
	QByteArray output; 
	int i = 0; 

	/* go to the begining of the SLIP packet */
	while((input.size() > i) && ((unsigned char)input.at(i) != SLIP_END))
	{
		i++;
	}

	if((input.size() > i) && ((unsigned char)input.at(i) == SLIP_END))
	{
		/* skip over the delimiter */
		i++; 

		/* and begin to decode the packet */
		while((input.size() > i) && ((unsigned char)input.at(i) != SLIP_END))
		{
			switch((unsigned char)input.at(i))
			{
				case SLIP_ESC:
					i++; 
					if((unsigned char)input.at(i) == SLIP_ESC_ESC)
					{
						output.append(SLIP_ESC);
					}
					else if((unsigned char)input.at(i) == SLIP_ESC_END)
					{
						output.append(SLIP_END);
					}
					break; 

				default:
					output.append(input.at(i));
					break; 
			}

			i++;	
		}
	}

	return output;
}
