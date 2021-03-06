/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/


// eigene Klasse fuer IO, die die systemunabhaengige Darstellung
// uebernimmt (bytes dreht, Character konvertiert)
// das Schreiben erfolgt aus Effizienzgruenden binaer
#ifndef _IO_HXX
#define _IO_HXX

#include <bf_svtools/bf_solar.h>

#ifdef UNX
#include <unistd.h>
#else
#include <io.h>
#endif

#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>


#ifndef _KEYCOD_HXX //autogen
#include <vcl/keycod.hxx>
#endif

#ifndef _STREAM_HXX //autogen
#include <tools/stream.hxx>
#endif
namespace binfilter {

/*$
class BinaryFile {
	int fd;
public:
	enum IO_OpenMode {
		BF_READ = O_RDONLY,
		BF_WRITE = O_RDWR,
		BF_CREATE = O_CREAT,
		BF_TRUNC = O_TRUNC
	};
		// ctor oeffnet File im BinearMode, dtor schliesst es
	BinaryFile(const String &, int eOpenMode);
	~BinaryFile();
	BOOL Ok() const {
		return -1 != fd;
	}
	operator int() const { return fd; }
};
*/

class SwIOin {
private:
		SvFileStream aStr; //$ ifstream
public:
	// Stream wird im entsprechenden Mode erzeugt.
	SwIOin(const String &rFilename, StreamMode nMode =
									STREAM_READ | STREAM_NOCREATE );

	SwIOin& operator>>(char& val);
	SwIOin& operator>>(unsigned char& val);
	SwIOin& operator>>(char* val);
	SwIOin& operator>>(unsigned char* val);
	SwIOin& operator>>(short& val);
	SwIOin& operator>>(unsigned short& val);
	SwIOin& operator>>(long& val);
	SwIOin& operator>>(unsigned long& val);
	String ReadString();
	KeyCode ReadKeyCode();
	// kann erweitert werden fuer weitere Arrays von
	// Basistypen; nLen ist die Anzahl der Elemente
	SwIOin& Read(char *buf, unsigned nLen);

	int operator!() { return aStr.GetError() != SVSTREAM_OK; }
		SvFileStream &operator()() {
		return aStr;
	}
};

class SwIOout {
private:
	void _write(const char *buf, unsigned size);
	SvFileStream aStr; //$ ofstream
public:
	// Stream wird im entsprechenden Mode erzeugt.
	SwIOout( const String &rFilename, StreamMode nMode =
									  STREAM_WRITE | STREAM_NOCREATE );
	SwIOout& operator<<(char val);
	SwIOout& operator<<(unsigned char val);
	SwIOout& operator<<(char* val);
	SwIOout& operator<<(unsigned char* val);
	SwIOout& operator<<(short val);
	SwIOout& operator<<(unsigned short val);
	SwIOout& operator<<(long val);
	SwIOout& operator<<(unsigned long val);
	SwIOout& operator<<(const String &);
	SwIOout& operator<<(const KeyCode &);
	// kann erweitert werden fuer weitere Arrays von
	// Basistypen; nLen ist die Anzahl der Elemente
	SwIOout& Write(const char *buf, unsigned nLen);

	int operator!() { return aStr.GetError() != SVSTREAM_OK; }
	SvFileStream &operator()() {
		return aStr;
	}
};


class SwIOinout {
private:
	SvFileStream aStr; //$ fstream

public:
	// Stream wird im entsprechenden Mode erzeugt.
	SwIOinout(const String &rFilename, StreamMode nMode =
									   STREAM_READWRITE  | STREAM_NOCREATE );

	SwIOinout& operator>>(char& val);
	SwIOinout& operator>>(unsigned char& val);
	SwIOinout& operator>>(char* val);
	SwIOinout& operator>>(unsigned char* val);
	SwIOinout& operator>>(short& val);
	SwIOinout& operator>>(unsigned short& val);
	SwIOinout& operator>>(long& val);
	SwIOinout& operator>>(unsigned long& val);
	String ReadString();
	KeyCode ReadKeyCode();
	// kann erweitert werden fuer weitere Arrays von
	// Basistypen; nLen ist die Anzahl der Elemente
	SwIOinout& Read(char *buf, unsigned nLen);
	SwIOinout& Read(unsigned short *buf, unsigned nLen );

	SwIOinout& operator<<(char val);
	SwIOinout& operator<<(unsigned char val);
	SwIOinout& operator<<(char* val);
	SwIOinout& operator<<(unsigned char* val);
	SwIOinout& operator<<(short val);
	SwIOinout& operator<<(unsigned short val);
	SwIOinout& operator<<(long val);
	SwIOinout& operator<<(unsigned long val);
	SwIOinout& operator<<(const String &);
	SwIOinout& operator<<(const KeyCode &);
	// kann erweitert werden fuer weitere Arrays von
	// Basistypen; nLen ist die Anzahl der Elemente
	SwIOinout& Write(const char *buf, unsigned nLen);

	int operator!() { return aStr.GetError() != SVSTREAM_OK; }
		SvFileStream &operator()() {
		return aStr;
	}

	BOOL Ok();
};



} //namespace binfilter
#endif

