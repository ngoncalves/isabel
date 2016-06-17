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
#include "isabelSerialize.h"
#include "json.h"

#include <QString>

/*--------------------- Private Variable Declarations ----------------*/

/*--------------------- Public Function Definitions ----------------*/
QByteArray serialize_encode(const QVariant& value)
{
	return QtJson::serialize(value);
}

QVariant serialize_decode(const QByteArray& value)
{
	return QtJson::parse(QString(value));
}

