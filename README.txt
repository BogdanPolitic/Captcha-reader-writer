	CAPTCHA Reader & Writer 2018
	============================

	The Captcha Reader & Writer has two functionalities:
		-> writing an image with a given captcha code, with the specified color
		-> reading from a captcha image and writing the digits into a .txt file, order starting from the left side of the image, and ending on the right side.

	The images format should be .bmp (bitmap). The digits in the captcha image are drawn in a 5x5 pixel space, like this:


	00000	    1	22222	33333	4   4	55555	66666	77777	88888	99999
	0   0		1	    2	    3	4   4	5		6			7	8	8	9	9
	0   0	    1	22222	33333	44444	55555	66666		7	88888	99999
	0   0	    1	2			3	    4		5	6	6		7	8	8		9
	00000	    1	22222	33333	    4	55555	66666		7	88888	99999


	The above representation contains the digits from 0 up to 9, each one fitting in a 5x5 space.
	Because the program needs to know a certain digit order that the captcha code should be translated into, there cannot be 2 digits which have the top-right corner in the same column of the image matrix. Thus, we cannot have this  kin of situation:


						55555
						5
						55555
							5
						55555

						66666
						6
						66666
						6   6
						66666


	Because of the translating incertitude ("The translated code is 56, or 65?").


	Each digit has a specific color, different than pure white (#000). The translation algorithm starts with recognizing the top-right corner of a specific digit, iterating on the image matrix columns, starting from the left, ending on the right side. One the top-right corner is recognized, it is easy to calculate a specific encoding for each digit.
	For example, let's assign L to the number of continuous pixels a digit has, starting from the top-right corner, including that one, and then going to the left side. Example for the '6' digit:

						XXXXX
						6
						66666			- the X are the continuous pixels, so L = 5.
							6
						66666

	For the '4' digit, L = 1.

	Let's assign D to the number of continuous pixels a digit has, starting from the top-right corner, including that one, and then going to the down side. Example for the '2' digit:

						2222X
							X
						2222X			- the X are the continuous pixels, so D = 3.
						2
						22222

	For the '7' digit, D = 5. For the '5' digit, D = 1.

	Now, let's assign N to the number of pixels a captcha digit representation has.
	Example for the '4' digit:

						Y	Y
						Y	Y
						YYYYY			- 11 'Y' pixels counted, so N = 11.
							Y
							Y

	For the '8' digit, N = 19.

	Let's assign the encoding E to the sum of the three parameters above: L, D, N.
	So E = L + D + N.
	It is not hard to notice that each digit has a different encoding E. Thus, we recognize the digit from the captcha image by simply looking to these 3 parameters.
	The algorithm used in this decoding & encoding program is based on the above described concept.


	The input text file should be named "input.txt" and should reside in the same directory with the executable. The format of the input file is the following:
		-> the first line names the input image name.
		-> the second line represents the color of the digits written to a new encoded captcha image code. (all the written digit representations have the same color)
		-> the fourth line does not matter.

	The decoded captcha code is residing in the file whose name ends in "_task2.txt".

	Input file examples are found in input/input_files. Input images examples are found in input/captcha_files .