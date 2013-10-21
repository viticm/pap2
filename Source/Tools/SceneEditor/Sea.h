#ifndef Sea_H
#define Sea_H

#include <windows.h>
#include <math.h>
namespace Water
{
#define DTOR		(PI/180.0f)             // Pi radians is 180 degrees
#define SQR(x)		(x*x)

/*
#define GRIDX       64				// size of water grid along X axis
#define GRIDZ       64				// size of water grid along Z axis
#define GRIDSIZE    GRIDX*GRIDZ		// total area of the water grid     */
#define GRIDSIZE_F  16384.0f		// total area of the water grid (as float)

#define N_WAVES		16				// number of wave components used
#define TIMESCALE   0.0025f			// slows the snimation speed

#define WINDSPEED	4.0f			// At 10m above the water surface (m/s)
#define WINDDIRECT	DTOR*0.0f		// Direction of the wind in radians

// Constants to reduce operations in equations
#define GRAVITY		9.81f
#define TWOPI       PI*2.0f
#define TWOPIG		TWOPI*GRAVITY
#define TWOPI_G     TWOPI/GRAVITY
#define GSQR        GRAVITY*GRAVITY   
#define GSQR_4      GSQR/4
#define PM_CONST    0.0081f*GSQR/pow(TWOPI,4)
#define SQRTTWOPI   sqrt(PI)/TWOPI


////////////////////////////////算法运算速度有待进一步优化////////////////////////////////

	// Definition of WAVE type
	typedef struct
	{
		float waveNo;    			// Wave number (identifier)
		float angFreq;				// Angular frequency of wave component
		float freq;					// Frequency of the wave component
		float direction;			// The direction the wave is travelling in
		float dirX, dirZ;			// Direction components in terms of axes
		float amplitude;			// Height of the wave
		float phase;				// Phase shift
	}WAVE;
class Sea
{
public:
	int m_GridX;       				// size of water grid along X axis
	int m_GridZ;       				// size of water grid along Z axis
	int m_GridSize;					// total area of the water grid
	
	WAVE wave[ N_WAVES ];			// All the wave components making up the surface
	Sea()
	{
		m_GridX = 0;
		m_GridZ = 0;
		m_GridSize = 0;

		WAVE *aWave  = &wave[ 0 ];
		nWaves   = 0;

		peakFreq = 1.25f / WINDSPEED;	// Peak frequency of a (PM) fully-developed sea

		while ( nWaves < N_WAVES )		// consider all the wave components in turn
		{
			float frequency = peakFreq + rand() / GRIDSIZE_F * 0.5f - 0.5f;

			if ( frequency > 0.0f )
			{
				aWave->waveNo     = (float)pow( TWOPI*frequency, 2.0f )/GRAVITY;
				aWave->angFreq    = (float)sqrt(GRAVITY * aWave->waveNo);
				aWave->freq       = aWave->angFreq/TWOPI;

				aWave->direction  = WINDDIRECT + ( rand() / GRIDSIZE_F * PI );
				aWave->dirX       = (float)cos( aWave->direction ) * 0.5f;
				aWave->dirZ       = (float)sin( aWave->direction ) * 0.5f;

				float amp = (float)calcAmp(aWave->freq, aWave->direction-WINDDIRECT, aWave->waveNo);
				float ang = rand() / GRIDSIZE_F * PI;
				aWave->amplitude = amp * (float)cos(ang);
				aWave->phase     = amp * (float)sin(ang);

				if ( fabs(aWave->amplitude) >= 0.01f ) {		// only consider larger waves
					aWave ++;									// Consider next wave component
					nWaves ++;									// A wave has been defined ok
				}
			}
		}
	}
	void Initialize(DWORD m_dwNumPoly_X, DWORD m_dwNumPoly_Z)
	{
		m_GridX = m_dwNumPoly_X + 1;
		m_GridZ = m_dwNumPoly_Z + 1;
		m_GridSize = m_dwNumPoly_X * m_dwNumPoly_Z;
	}
	void animate()
	{
		float time = timeGetTime()*TIMESCALE;      // slow animation speed
		float h;									// height
		float x, z;									// normal variables 
		int a=0, b=0, i=0;							// loop indices

		for( a=0; a<m_GridZ; a++ ) {
			for( b=0; b<m_GridX; b++ ) {
				h = 0.0f;

				// Consider each wave component to find overall effect
				for ( i=0; i<nWaves; i++ )
				{
					h += wave[i].amplitude * 
						(float)sin( wave[i].phase + wave[i].angFreq * 
						wave[i].freq *
						(time + wave[i].dirX*b + wave[i].dirZ*a));
				}
				waterHeight[b + a*m_GridX] = h;
			}
		}

		for( a=0; a<m_GridZ; a++ ) {
			for( b=0; b<m_GridX; b++ ) {

				x = waterHeight[wrapXaxis(b+1)+a*m_GridX]
				- waterHeight[wrapXaxis(b-1)+a*m_GridX];
				z = waterHeight[b+wrapZaxis(a+1)*m_GridX]
				- waterHeight[b+wrapZaxis(a-1)*m_GridX];

				waterNormal[b+a*m_GridX].x = x;
				waterNormal[b+a*m_GridX].y = 4.0f;
				waterNormal[b+a*m_GridX].z = z;
			}
		}
	}
	void release()
	{
//		delete(wave[ N_WAVES ]);
	}


	// Sea variables
	float peakFreq;
	int   nWaves;

	// Represent the water as a 2D array
	float  waterHeight[16384];
	D3DXVECTOR3 waterNormal[16384];

	double LogGamma ( double fX )
	{
		static const double s_afCoeff[6] =
		{
			76.18009173f, -86.50532033f, 24.01409822f, -1.231739516f,
				0.120858003e-2f, -0.536382e-5f
		};

		fX -= 1.0f;
		double fTmp = fX + 5.5f;
		fTmp -= (fX+0.5f)*log(fTmp);
		double fSeries = 1.0f;
		for (int j = 0; j <= 5; j++)
		{
			fX += 1.0f;
			fSeries += s_afCoeff[j]/fX;
		}
		return -fTmp + log(2.50662827465f*fSeries);
	}
	double Gamma    ( double fX )
	{
		return exp( LogGamma(fX) );
	}

	double PM( double f )									// E(f)
	{		
		return (PM_CONST/pow(f,5)) *	// PM_CONST = 0.0081f*GSQR/pow(TWOPI,4)
			(exp(-0.24*pow(TWOPI_G*f*WINDSPEED,-4)));
	}

	double DirectionalSpreading ( double f, double theta )	// G(f,theta)
	{
		double s=5.0f;					// constant power

		double angle = cos(theta/2);    // Wave angle
		angle =  pow(angle,2*s);        // cos^2s

		return SQRTTWOPI * (Gamma(s+1)/Gamma(s+0.5)) * angle;
	}
	double EnergyDist( double f, double theta )			// E(f,theta)
	{
		return PM(f) * DirectionalSpreading(f, theta);
	}

	double calcAmp( double f, double theta, double k )
	{
		double omega = 10.0;            // Water sample area
		return sqrt( GSQR_4*EnergyDist(f,theta)/(omega*pow(f,3)) );
	}

	// ensure the grid calculations don't go out of bounds
	int    wrapXaxis( int x )
	{			// handle the water edges in x axis
		if( x<0 )          return (x+m_GridX);
		else if (x>=m_GridX) return (x-m_GridX);
		else               return x;
	}
	int    wrapZaxis( int z )
	{			// handle the water edges in z axis
		if( z<0 )          return (z+m_GridZ);
		else if (z>=m_GridZ) return (z-m_GridZ);
		else               return z;
	}
};
};

#endif