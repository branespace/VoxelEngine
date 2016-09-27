#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glew.h>

#include <glfw3.h>


GLuint loadBMP(const char * imagepath){

	printf("Reading image %s\n", imagepath);

	//Read image header
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	//RGB pixel data
	unsigned char * data;

	//Open the file
	FILE * file = fopen(imagepath,"rb");
	if (!file) //File doesn't exist
	{
		printf("%s could not be opened.\n", imagepath); 
		getchar(); 
		return 0;
	}

	//Read the header, and bail if too short
	if ( fread(header, 1, 54, file)!=54 ){ 
		printf("Invalid BMP\n");
		return 0;
	}
	//BM files start with "BM"
	if ( header[0]!='B' || header[1]!='M' ){
		printf("Invalid BMP\n");
		return 0;
	}
	//Expect 24 bpp
	if ( *(int*)&(header[0x1E])!=0  )         {printf("Invalid BMP\n");    return 0;}
	if ( *(int*)&(header[0x1C])!=24 )         {printf("Invalid BMP\n");    return 0;}

	//Read image data
	dataPos    = *(int*)&(header[0x0A]);
	imageSize  = *(int*)&(header[0x22]);
	width      = *(int*)&(header[0x12]);
	height     = *(int*)&(header[0x16]);

	//Correct misformatted file
	if (imageSize==0)    imageSize=width*height*3; //24 bits = 3 bytes per pixel
	if (dataPos==0)      dataPos=54;//Header is 54 bits

	//Build a buffer
	data = new unsigned char [imageSize];

	//Read file data
	fread(data,1,imageSize,file);

	//Close file
	fclose (file);

	//Generate texture
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	//Pass texture to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	//Clean up memory
	delete [] data;

	//Basic filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	glGenerateMipmap(GL_TEXTURE_2D);

	return textureID;
}

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

GLuint loadDDS(const char * imagepath){

	unsigned char header[124];

	FILE *fp; 
 
	//Open file
	fp = fopen(imagepath, "rb"); 
	if (fp == NULL){
		printf("%s could not be opened.\n", imagepath); getchar(); 
		return 0;
	}
   
	//Read first bits for file identification
	char filecode[4]; 
	fread(filecode, 1, 4, fp); 
	if (strncmp(filecode, "DDS ", 4) != 0) { 
		fclose(fp); 
		printf("%s is incorrectly formatted\n", imagepath);
		return 0; 
	}
	
	//Get surface data
	fread(&header, 124, 1, fp); 

	unsigned int height      = *(unsigned int*)&(header[8 ]);
	unsigned int width	     = *(unsigned int*)&(header[12]);
	unsigned int linearSize	 = *(unsigned int*)&(header[16]);
	unsigned int mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC      = *(unsigned int*)&(header[80]);
	unsigned char * buffer;
	unsigned int bufsize;

	//Check size including mipmaps
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize; 
	buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char)); 
	fread(buffer, 1, bufsize, fp); 
	//And close the file
	fclose(fp);

	//Set format
	unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4; 
	unsigned int format;
	switch(fourCC) 
	{ 
	case FOURCC_DXT1: 
		format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; 
		break; 
	case FOURCC_DXT3: 
		format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; 
		break; 
	case FOURCC_DXT5: 
		format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; 
		break; 
	default: 
		free(buffer); 
		return 0; 
	}

	//Build and bind texture
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);	
	
	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
	unsigned int offset = 0;

	//Load mipmaps
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) 
	{ 
		unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize; 
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,  
			0, size, buffer + offset); 
	 
		offset += size; 
		width  /= 2; 
		height /= 2; 

		//Non-power of 2?
		if(width < 1) width = 1;
		if(height < 1) height = 1;
	} 

	//Clean up
	free(buffer); 

	printf("%s loaded and validated.\n", imagepath);

	return textureID;
}