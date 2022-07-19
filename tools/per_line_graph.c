#include <stdio.h>

#define CNFG_IMPLEMENTATION

#include "rawdraw_sf.h"


/*

Treats each new line as a new frame.  Graphs all the numbers on this line.

Example
gcc -o per_line_graph per_line_graph.c  -lX11 && stty -F /dev/ttyUSB0 115200 -echo raw | cat /dev/ttyUSB0 | tr -s ' ' | cut -d ' ' -f 4- | ./per_line_graph 

*/

void HandleKey( int keycode, int bDown ) { }
void HandleButton( int x, int y, int button, int bDown ) { }
void HandleMotion( int x, int y, int mask ) { }
void HandleDestroy() { }
int main()
{
	CNFGSetup( "per-line grapher", 1024, 768 );
	while(CNFGHandleInput())
	{
		CNFGBGColor = 0x000080ff; //Dark Blue Background

		short w, h;
		CNFGClearFrame();
		CNFGGetDimensions( &w, &h );


		char line[8192];
		fgets(line, sizeof(line), stdin);
		
		int field = 0;
		char * fieldstart = 0;
		double fields[8192];
		for( char * c = line; ; c++ )
		{
			if( *c == ' ' || *c == '\t' || *c == ':' || *c == 0 || *c == '\r' || *c == '\n' )
			{
				if( fieldstart )
				{
					if( c - fieldstart >= 1 )
					{
						fields[field++] = atof( fieldstart );
					}
					fieldstart = 0;
				}
				if( !*c || *c == '\r' || *c == '\n' ) break;
			}
			else
			{
				if( !fieldstart ) fieldstart = c;
			}
		}
		if( field == 0 ) continue;
		int fdiv = w/field;

		int i;
		double fdmin = 1e20;
		double fdmax = -1e20;
		for( i = 0; i < field; i++ )
		{
			if( fields[i] < fdmin ) fdmin = fields[i];
			if( fields[i] > fdmax ) fdmax = fields[i];
		}

		//Change color to white.
		CNFGColor( 0xffffffff ); 
		
		fdmin--;
		fdmax++;
		printf( "%f %f %f\n", fdmin, fdmax, fdmax-fdmin );
		for( i = 1; i < field; i++ )
		{
			double last = h - 1 - (fields[i-1] - fdmin)/(fdmax - fdmin)*(h-1);
			double next = h - 1 - (fields[i] - fdmin)/(fdmax - fdmin)*(h-1);
			CNFGTackSegment( fdiv*i, last, fdiv*(i+1), next );	
		}

		//Display the image and wait for time to display next frame.
		CNFGSwapBuffers();		
	}
}
