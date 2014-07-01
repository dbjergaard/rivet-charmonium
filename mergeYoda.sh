yodamerge -o 1S0_8.yoda *1S0\(8\)*.yoda
yodamerge -o 3PJ_1.yoda *3PJ\(1\)*.yoda
yodamerge -o 3PJ_8.yoda *3PJ\(8\)*.yoda
test -f *3S1\(1\)*.yoda || mv *3S1\(1\)*.yoda 3S1_1.yoda 
yodamerge -o 3S1_8.yoda *3S1\(8\)*.yoda
yodamerge -o ColorSinglet.yoda *\(1\)*.yoda
yodamerge -o ColorOctet.yoda *\(8\)*.yoda

yodamerge -o P6_1S0_8.yoda {423,426,429}.yoda
yodamerge -o P6_3PJ_1.yoda {431..439}.yoda
yodamerge -o P6_3PJ_8.yoda {424,427,430}.yoda
test -f 421.yoda || mv 421.yoda P6_3S1_1.yoda
yodamerge -o P6_3S1_8.yoda {422,425,428}.yoda
yodamerge -o P6_ColorSinglet.yoda {431..439}.yoda 421.yoda
yodamerge -o P6_ColorOctet.yoda {422..430}.yoda
