
long EncPositionCounter = 0;//position counter
char EncState = -1;//encoder prev state

/**
 * @author Kyrylov Andrii
 * @todo scan f1, f2, f3 signals and count current position in ticks
 */
void EncoderScan(void)
{
    TRISBbits.TRISB0=1;//input F1.1
    TRISBbits.TRISB1=1;//input F1.2
    
char New;
New = PORTB & 0x03;
//impuls investigation
switch(EncState)
	{
	case 2:
		{
		if(New == 3) EncPositionCounter--;
		if(New == 0) EncPositionCounter++;
		break;
		}

	case 0:
		{
		if(New == 2) EncPositionCounter--;
		if(New == 1) EncPositionCounter++;
		break;
		}
	case 1:
		{
		if(New == 0) EncPositionCounter--;
		if(New == 3) EncPositionCounter++;
		break;
		}
	case 3:
		{
		if(New == 1) EncPositionCounter--;
		if(New == 2) EncPositionCounter++;
		break;
		}
	}
EncState = New;		// ?????????? ????? ????????				// ??????????? ?????????
}

long EncStartVcounter = -1;
int Vvalue = 0;

/**
 * @author Kyrylov Andrii
 * @todo count speed from position
 */
void EncCountV()
{
    if(EncStartVcounter == -1)
    {
        EncStartVcounter = EncPositionCounter;
    }
    Vvalue= EncPositionCounter - EncStartVcounter;
    EncStartVcounter = EncPositionCounter;
}

/**
 * @author Kyrylov Andrii
 * @param *str pointer to output string
 * @todo convert current distance to string
 * @return offset in str
 */
char EncGetDistanceStr(char *str)
{
    long marks = (EncPositionCounter>>2);
    long distance = marks*_distancePerMarks;//travel distance
    char offset = LongToString(distance,str);//view distance
    return offset;
}

/**
 * @author Kyrylov Andrii
 * @todo convert current counter to distance in mm
 * @return distance in mm
 */
long EncGetDistanceLong()
{
    long marks = (EncPositionCounter>>2); 
    long distance = marks*_distancePerMarks;//travel distance
    return distance;
}

long EncDistanceToCounter(long distance)
{
    long counter = distance/_distancePerMarks;
    counter<<=2;
    return counter;
}

int EncGetV()
{
    int speed = Vvalue>>2;
    speed*=_distancePerMarks * _vCoefficient;
    return speed/0.0708;
}