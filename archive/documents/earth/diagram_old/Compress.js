/*
//http://brett.sourceforge.net/Compression/Compress.js
function Compress(value)
{
	this.remainder = 0;
	
	String.prototype.toNumber = function () {
		if (this.length == 1) {
			return String.charCodeAt(this[0]);
		}
		return this[0].toNumber << bitsPerUnicodeCharacter + this.slice(1).toNumber();
	}

	function Compress(value)
	{
		compressed = (
				Compress(value.substr(0, value.length - 1)) 
				+ Encode(value[0])
			) << Exponent(this.remainder) + this.remainder;
		
		if (compressed == value)
		{
			return String.toNumber(compressed);
		}
		else
		{
			return Compress(String.fromCharCode(1) + compressed)
		}
	}

	function Inflate(value)
	{ 
		inflated = Inflate(value.substr(0, value.length - 1))
			+ Decode(value[0]) + Inflate(value.slice[1]);
		if (inflated.)
	}
		//while (value = Compress(value) != value)
		// Compress until we can't compress any more,
		// recording the number of times we compress
		// at the end of each compression
		// Since we then compress the result, this last
		// number can only always be "1""
		for (	compressedValue = value, value = "";
				compressedValue != value;
				compressedValue = (value = "1" + Compress(Number(Exponent(value), Number(value))))
			)
		;
		return compressedValue;
	}
	
	function Inflate(value)
	{
		if (value.length < bitsPerChunk) {
			return value;
		}
		
		if (value[0] == String.fromCharCode("1")) {
			
		}
		//while (value[0] == "1")
			
		// Inflate while inflation starts with 1 and decode 
	}
	
	// Read a chunk, (a unicode character)
	// Compress it and append it to the compressed value
	function Compress(value) {

		for (	compressedValue = "", index = 0; 
				index < value.length; 
				compressedValue += 
					Compress(value.charCodeAt(index++))
			)
		;

		// Compression
		// '0'
		// 
		// 
		// Inflation (reading bits from a number)
		// ReadNumber(value)
		// '0'
		//		exponent = ReadNumber(value << 1)
		//		number = read exponent bits
		//		return number
		// '1'
		//		return 1
		// <end of value while reading number>
		//		return value
		//		
		// Write Number(value)
		// 
		// Below is an example of numbers, with ':' to show 
		// the exponent vs the number
		// 
		// Number	Compressed
		// 0		:1	
		// 1		010:10
		// 2		010:11
		// 3		010:100
		// 4		010:101
		// 5		010:110
		// 6		010:111
		// 7		011:100
		// (...) 
		// 8 - 15   7 bits
		// 16 - 31  8 bits
		// 32 - 63  9 bits
		// 64 - 127 10 bits
		// (...)
		// 254		0100:1000:11111111 (15 bits)
		// If we count the number of bits required to count from 0->7,
		// we only need 34 bits
		// If we used traditional encoding, to store numbers 0-7 would require
		// 4 bits, which would require 8*4 = 32 bits
		// However, most the numbers (0-6), use less then the 4 bits traditionally
		// required to encode them.
		function Compress(value)
		{
			
		}
		return compressedValue;
	}
	
	this.Inflate = function() {
		return value;
	}
	
	leastSignificantBitMask = integer.parseInt("0000000000000001");
	mostSignificantBitMask = integer.parseInt("1000000000000000");
	bitsPerChunk = bitsPerCharacterCode = 16; // NB this is javascripts implementation 
	//							  of charCodeAt returning a unicode number
	return this;
}

compressedValue = Compress("Hello World");
alert(compressedValue.Inflate());


*/
bitsPerCharacterCode = 16;

	String.prototype.toNumber = function () {
		if (this == "") 
			return 0;
		else
			return this.substr(0, this.length - 1).toNumber() << bitsPerCharacterCode + this.charCodeAt(this.length - 1);
	}

test = String.fromCharCode(parseInt("000", 2), parseInt("111", 2), parseInt("000", 2), parseInt("111", 2));
alert(0 << bitsPerCharacterCiode);
//alert(test.toNumber() == parseInt("000111000111", 2))
