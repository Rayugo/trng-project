// omp_atomic.cpp
// compile with: /openmp

#include "trng.h"

#define MAX 4

int TRNG::getRandomByte()
{
        long double r = 4.0;
        long double x = 0.36089632;
        long double y[4] = {};

        int numOfNetworks = 3; //alpha
        int numOfStages = 3; //beta
        int numOfIter = 2; //gamma


        uint8_t output = 0;

    #pragma omp parallel num_threads(MAX)
        {
            int count = 0;

            while (count != numOfNetworks) //multiple instances of while loop is computed in parallel
            {
                ++count;
                long double rPrim[4] = { r, r, r, r };

                for (int i = 0; i < 4; i++)
                {
                    for (int j = 0; j < numOfIter; j++)
                    {
                        x = x * rPrim[1] * (1 - x);
                        x = (x * 1000) - floor(x * 1000);
                    }
                    y[i] = 3.86 + (x * 0.14);

                }

                for (int k = 1; k < numOfStages; k++)
                {
                    if (x >= 0.5) //omega network
                    {
                        rPrim[0] = (y[0] + y[2]) / 2;
                        rPrim[1] = (y[0] + y[2]) / 2;

                        rPrim[2] = (y[1] + y[3]) / 2;
                        rPrim[3] = (y[1] + y[3]) / 2;
                    }
                    else //clos network
                    {
                        rPrim[0] = (y[0] + y[1] + y[2] + y[3]) / 4;
                        rPrim[1] = (y[0] + y[1] + y[2] + y[3]) / 4;
                        rPrim[2] = (y[0] + y[1] + y[2] + y[3]) / 4;
                        rPrim[3] = (y[0] + y[1] + y[2] + y[3]) / 4;
                    }

                    for (int i = 0; i < 4; i++)
                    {
                        for (int j = 0; j < numOfIter; j++)
                        {
                            x = x * rPrim[i] * (1 - x);
                            x = (x * 1000) - floor(x * 1000);
                        }
                        y[i] = 3.86 + (x * 0.14);
                    }
                }
                r = (y[0] + y[1] + y[2] + y[3]) / 4;
            }
        }
        
        output = static_cast<uint8_t>(x * 256);
        return output;
}

void TRNG::GenerateBlock(CryptoPP::byte* output, size_t size)
{
    for (size_t i = 0; i < size; ++i) {
        output[i] = getRandomByte();
    }
}


