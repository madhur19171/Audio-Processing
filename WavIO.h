#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
using namespace std;

class WaveIO
{
public:
   int NumChannels;    //Stores the number of channels (1-Mono, 2- Stereo)
   int SamplingRate;   //Stores the Sampling Rate of the audio file;
   int DataLength;     //Stores the number of Bytes of audio data ie. 2 * Number of samples.
   int BitsPerSample;  //Size of each sample in bit. Usually 16-bit
   vector<char> meta;  //For Storing The meta data of Wav File
   vector<short> data; //For storing the Audio Data in an array
   FILE *fp;

   //Constructor for reading a file
   //name is the name of the file to be read
   //The file should be in the current working directory
   WaveIO(char name[])
   {
      fp = fopen(name, "rb"); // read binary data mode

      if (fp == NULL)
      {
         perror("Error while opening the file.\n");
         exit(EXIT_FAILURE);
      }

      //Initialization
      NumChannels = 0;
      SamplingRate = 0;
      DataLength = 0;
      BitsPerSample = 0;

      //Pushing the Metadata into an array for easy Indexed Access of each element
      for (int j = 0; j < 44; j++)
      {
         meta.push_back(fgetc(fp));
      }

      NumChannels = (meta[23] << 8) + meta[22]; //Number Of channels in audio file. Mono = 1, Stereo = 2.....etc.
      SamplingRate = (meta[27] << 24) + (meta[26] << 16) + (meta[25] << 8) + meta[24];
      DataLength = (meta[43] << 24) + (meta[42] << 16) + (meta[41] << 8) + meta[40]; //Length of Audio Data in bytes(2 * Number of Samples)
      BitsPerSample = (meta[35] << 8) + meta[34];                                    //Size of each Sample. 8-bit or 16-bit
      printf("Number Of Channels: %d \nSampling Rate: %d \nLength Of Data: %d\nBits Per Sample: %d\n",
             NumChannels, SamplingRate, DataLength, BitsPerSample);
   }

   //Constructor for writing a file
   /*
      This Constructor stores the necessary information regarding the audio file
      in the global variables. So all data is stored at the time of Creation of object.
    */
   //name is the name of output file. A ".wav" extension is added.
   /*
   NC is the number of channels in the output audio file.
   SR is the sampling rate.
   BPS is the Bits Per Sample
   d is the data vector.
    */
   WaveIO(char name[], int NC, int SR, int BPS, vector<short> d)
   {
      fp = fopen(name, "wb"); // Write binary data mode

      NumChannels = NC;
      SamplingRate = SR;
      BitsPerSample = BPS;
      DataLength = 2 * d.size();
      data = d;

      // For More Information on Wav File Format, Visit - http://soundfile.sapp.org/doc/WaveFormat/
      // Visit this site for details of all the fields.
      int Subchunk2Size = DataLength;
      int Subchunk2ID = stbin(0x64617461, 4);
      short BitsPerSample = BPS;
      short BlockAlign = NumChannels * BPS / 8;
      int ByteRate = SamplingRate * NumChannels * BitsPerSample / 8;
      short AudioFormat = 1;
      int Subchunk1Size = 16;
      int Subchunk1ID = stbin(0x666d7420, 4);
      int Format = stbin(0x57415645, 4);
      int ChunkSize = 36 + DataLength;
      int ChunkID = stbin(0x52494646, 4);
      short NCW = NumChannels;
      int SRW = SamplingRate;
      short BPSW = BitsPerSample;

      fwrite(&ChunkID, sizeof(ChunkID), 1, fp);
      fwrite(&ChunkSize, sizeof(ChunkSize), 1, fp);
      fwrite(&Format, sizeof(Format), 1, fp);
      fwrite(&Subchunk1ID, sizeof(Subchunk1ID), 1, fp);
      fwrite(&Subchunk1Size, sizeof(Subchunk1Size), 1, fp);
      fwrite(&AudioFormat, sizeof(AudioFormat), 1, fp);
      fwrite(&NCW, sizeof(NCW), 1, fp);
      fwrite(&SRW, sizeof(SRW), 1, fp);
      fwrite(&ByteRate, sizeof(ByteRate), 1, fp);
      fwrite(&BlockAlign, sizeof(BlockAlign), 1, fp);
      fwrite(&BPSW, sizeof(BPSW), 1, fp);
      fwrite(&Subchunk2ID, sizeof(Subchunk2ID), 1, fp);
      fwrite(&Subchunk2Size, sizeof(Subchunk2Size), 1, fp);

      for (short val : d)
      {
         fwrite(&val, sizeof(val), 1, fp);
      }
   }

   int stbin(int dat, int x)
   {
      if (x == 2)
      {
         return ((dat & 255) << 8 + (dat >> 8));
      }
      else
      {
         /* code */
         int fir = dat & 255;
         dat = dat >> 8;
         int sec = dat & 255;
         dat = dat >> 8;
         int tir = dat & 255;
         dat = dat >> 8;
         int fou = dat;
         return ((fir << 24) + (sec << 16) + (tir << 8) + fou);
      }
   };

   int readWav()
   {

      // For More Information on Wav File Format, Visit - http://soundfile.sapp.org/doc/WaveFormat/

      int count = 0;
      for (int j = 0; j < DataLength / 2; j++, count++)
      {
         short ch1;
         fread(&ch1, sizeof(short), 1, fp);
         data.push_back(ch1); /*Since this is a single channel audio file, both channels have to be combined.
                                                Since this was a 16-bit audio, the first byte represented the left channel and
                                                Second byte represented the right channel, so both channels had to be combined
                                                with right channel as the MSB and left channel as LSB.
                                                    */
      }

      //Printing the Wav Data
      count = 0;
      for (int j = 0; j < 40; j++)
      {
         count++;
         printf("%d ", data[j]);
         if (count % 10 == 0)
            printf("\n");
      }

      fclose(fp);
      return 0;
   }
};
