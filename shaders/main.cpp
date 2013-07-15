#include <iostream>
#include "SDL.h"
#include "GL\glew.h"
#include <vector>
#include <string>
#include <fstream>
#include "helpers.h"

GLuint g_vertexHandle, g_indicesHandle, g_colorHandle, g_textureHandle;
GLuint g_program;
GLuint g_textureId;

namespace
{
    void init()
    {
        if (SDL_Init(SDL_INIT_VIDEO) == 0)
        {
            SDL_Surface* pScreen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_OPENGL | SDL_DOUBLEBUF );
            if ( !pScreen )
            {
                throw std::exception("SDL_SetVideoMode failed to return a valid surface");
            }
            SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
            SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
            SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
            SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
            SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
            glewInit();
        }
        else
        {
            throw std::exception("Can't initialization SDL video");
        }
    }

    void glGenerateVBO()
    {
        GLfloat vertices[] = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 1.0f, 0.0f };
        GLfloat color[] = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat texture[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f };
        GLubyte indices[] = { 0, 1, 2 };
        glGenBuffers( 1, &g_vertexHandle );
        glGenBuffers( 1, &g_indicesHandle );
        glGenBuffers( 1, &g_colorHandle );
        glGenBuffers( 1, &g_textureHandle );
        glBindBuffer( GL_ARRAY_BUFFER, g_vertexHandle );
        glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
        glBindBuffer( GL_ARRAY_BUFFER, g_colorHandle );
        glBufferData( GL_ARRAY_BUFFER, sizeof( color ), color, GL_STATIC_DRAW );
        glBindBuffer( GL_ARRAY_BUFFER, g_textureHandle );
        glBufferData( GL_ARRAY_BUFFER, sizeof( texture ), texture, GL_STATIC_DRAW );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, g_indicesHandle );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( indices ), indices, GL_STATIC_DRAW );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }

    void glRender()
    {
        glClearColor( 0.0f, 0.0f, 1.0f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glBindBuffer( GL_ARRAY_BUFFER, g_vertexHandle );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
        glBindBuffer( GL_ARRAY_BUFFER, g_colorHandle );
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0 );
        glBindBuffer( GL_ARRAY_BUFFER, g_textureHandle );
        glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, 0 );
        glEnableVertexAttribArray( 0 );
        glEnableVertexAttribArray( 1 );
        glEnableVertexAttribArray( 2 );
        glEnable( GL_TEXTURE_2D );
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, g_textureId );

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, g_indicesHandle );
        glDrawElements( GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, 0 );
        glDisableVertexAttribArray( 0 );
        glDisableVertexAttribArray( 1 );
        glDisable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, 0 );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    }

    void readFile( const std::string& fileName, std::vector<char>& source )
    {
        const int MAX_LINE_SIZE = 1024;
        char line[MAX_LINE_SIZE];
        std::fstream file( fileName.c_str() );
        while ( !file.eof() )
        {
            file.getline(line, MAX_LINE_SIZE);
            for (char* ptr = line; *ptr != 0; ptr++)
            {
                source.push_back( *ptr );
            }
            source.push_back( '\n' );
        }
        source.push_back('\0');
    }

    static void show_info_log( GLuint object, PFNGLGETSHADERIVPROC glGet__iv, PFNGLGETSHADERINFOLOGPROC glGet__InfoLog )
    {
        GLint log_length;
        char *log;

        glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
        log = (char *)malloc(log_length);
        glGet__InfoLog(object, log_length, NULL, log);
        fprintf(stderr, "%s", log);
        free(log);
    }

    void glInstallShaders()
    {
        GLuint vertShader = glCreateShader( GL_VERTEX_SHADER );
        GLuint fragShader = glCreateShader( GL_FRAGMENT_SHADER );
        GLuint program = glCreateProgram();
        std::vector<char> vertSource;
        std::vector<char> fragSource;
        readFile( "shader.vert", vertSource );
        readFile( "shader.frag", fragSource );
        const char* vertPointers[] = {&vertSource[0]};
        const char* fragPointers[] = {&fragSource[0]};
        glShaderSource( vertShader, 1, vertPointers, 0 );
        glShaderSource( fragShader, 1, fragPointers, 0 );
        glCompileShader( vertShader );
        glCompileShader( fragShader );

        //Debug - check compilation
        GLint shaderOk;
        glGetShaderiv(vertShader, GL_COMPILE_STATUS, &shaderOk);
        if (!shaderOk) {
            fprintf(stderr, "Failed to compile %s:\n", "shader.vert");
            show_info_log(vertShader, glGetShaderiv, glGetShaderInfoLog);
            glDeleteShader(vertShader);
        }
        glGetShaderiv(fragShader, GL_COMPILE_STATUS, &shaderOk);
        if (!shaderOk) {
            fprintf(stderr, "Failed to compile %s:\n", "shader.frag");
            show_info_log(fragShader, glGetShaderiv, glGetShaderInfoLog);
            glDeleteShader(fragShader);
        }


        glAttachShader( program, vertShader );
        glAttachShader( program, fragShader );
        glLinkProgram( program );
        glUseProgram( program );
        g_program = program;
    }

    void glLoadTextures()
    {
        g_textureId = loadTexture( "texture.bmp" );
        GLuint textureLocation = glGetUniformLocation( g_program, "texture" );
        glUniform1i(textureLocation, 0);
    }

    void runEventLoop()
    {
        SDL_Event evt;
        bool fExit = false;
        while ( !fExit )
        {
            while ( SDL_PollEvent( &evt ) )
            {
                switch ( evt.type )
                {
                    case SDL_QUIT: fExit = true;
                        break;
                    default: break;
                }
            }
            glRender();
            SDL_GL_SwapBuffers();
        }
    }
}

int main( int argc, char* argv[] )
{
    try
    {
        init();
        glInstallShaders();
        glLoadTextures();
        glGenerateVBO();
        runEventLoop();
    }
    catch (std::exception& ex)
    {
       std::cout<<"Failure due to exception \n";
    }      
    return 0;
}