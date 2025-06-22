#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_set>
#include <sstream>
#include <algorithm>
#include <stdint.h>
#include <cctype>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>

typedef uint32_t u32;
typedef int32_t s32;

std::unordered_set<std::string> PositiveWords = {
    "good", "great", "excellent", "amazing", "wonderful", "fantastic",
    "awesome", "brilliant", "perfect", "beautiful", "love", "like",
    "enjoy", "happy", "joy", "pleased", "satisfied", "delighted",
    "thrilled", "excited", "positive", "best", "better", "superior",
    "outstanding", "remarkable", "impressive", "incredible", "magnificent",
    "marvelous", "splendid", "superb", "terrific", "fabulous",
    "success", "successful", "win", "victory", "achieve", "accomplish"
};

std::unordered_set<std::string> NegativeWords = {
    "bad", "terrible", "awful", "horrible", "disgusting", "hate",
    "dislike", "sad", "angry", "upset", "frustrated", "disappointed",
    "depressed", "miserable", "unhappy", "annoyed", "irritated",
    "furious", "enraged", "worst", "worse", "inferior", "poor",
    "pathetic", "useless", "worthless", "stupid", "dumb", "idiotic",
    "fail", "failure", "lose", "loss", "defeat", "disaster",
    "problem", "issue", "trouble", "difficulty", "wrong", "mistake"
};

std::unordered_set<std::string> NegationWords = {
    "not", "cannot", "can't", "won't", "wouldn't", "shouldn't", "couldn't",
    "don't", "doesn't", "didn't", "isn't", "aren't", "wasn't", "weren't",
    "hasn't", "haven't", "hadn't", "without", "dont"
};

static std::string
ToLowerCase(
    const std::string& str
    )
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);

    return result;
}

static u32
IsPositiveWord(
    std::string Word
    )
{
    std::string Lower = ToLowerCase(Word);
    return PositiveWords.find(Lower) != PositiveWords.end();
}

static u32
IsNegativeWord(
    std::string Word
    )
{
    std::string Lower = ToLowerCase(Word);
    return NegativeWords.find(Lower) != NegativeWords.end();
}

static u32
IsNegationWord(
    std::string Word
    )
{
    std::string Lower = ToLowerCase(Word);
    return NegationWords.find(Lower) != NegationWords.end();
}

enum token_type
{
    TOKEN_TYPE_POSITIVE,
    TOKEN_TYPE_NEGATIVE,
    TOKEN_TYPE_NEGATION,
    TOKEN_TYPE_IDENTIFIER,
};

const char *TokenTypes[] =
{
    "TOKEN_TYPE_POSITIVE",
    "TOKEN_TYPE_NEGATIVE",
    "TOKEN_TYPE_NEGATION",
    "TOKEN_TYPE_IDENTIFIER",
};

struct token
{
    token_type Type;

    std::string String;
    u32 StringLength;
};

enum emotion_state
{
    EMOTION_NONE,
    EMOTION_NEGATE,
};

struct tokenizer
{
    const char *Data;
    u32 InputPosition;
    std::vector<token> Tokens;

    emotion_state EmotionState;
};

static void
ResetEmotionState(
    tokenizer *Tokenizer
    )
{
    Tokenizer->EmotionState = EMOTION_NONE;
}

static tokenizer
InitializeTokenizer(
    const char *InputString
    )
{
    tokenizer Result;

    Result.Data = InputString;
    Result.InputPosition = 0;
    Result.EmotionState = EMOTION_NONE;

    return Result;
}

static u32
TokenizerIsEof(
    tokenizer *Tokenizer
    )
{
    return Tokenizer->Data[Tokenizer->InputPosition] == '\0';
}

static char
TokenierCurrentCharacter(
    tokenizer *Tokenizer
    )
{
    return Tokenizer->Data[Tokenizer->InputPosition];
}

static void
SkipWhitespace(
    tokenizer *Tokenizer
    )
{
    char C = TokenierCurrentCharacter(Tokenizer);
    while(C == ' ' || C == '\t' || C == '\n' || C == ',' || C == '.')
    {
        Tokenizer->InputPosition++;
        C = TokenierCurrentCharacter(Tokenizer);
    }
}

static u32
IsInputCharacterValid(
    char C
    )
{
    if((C >= 'a' && C <= 'z') || (C >= 'A' && C <= 'Z') || (C >= '0' && C <= '9'))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static char
TokenizerAdvance(
    tokenizer *Tokenizer
    )
{
    return Tokenizer->Data[Tokenizer->InputPosition++];
}

static void
PushToken(
    tokenizer *Tokenizer,
    const char *String,
    u32 Length
    )
{
    std::string Match = std::string(String, Length);
    token Result;


    if(IsPositiveWord(Match))
    {
        if(Tokenizer->EmotionState == EMOTION_NEGATE)
        {
            Result.Type = TOKEN_TYPE_NEGATIVE;
        }
        else
        {
            Result.Type = TOKEN_TYPE_POSITIVE;
        }
        ResetEmotionState(Tokenizer);
    }
    else if(IsNegativeWord(Match))
    {
        if(Tokenizer->EmotionState == EMOTION_NEGATE)
        {
            Result.Type = TOKEN_TYPE_POSITIVE;
        }
        else
        {
            Result.Type = TOKEN_TYPE_NEGATIVE;
        }

        Result.Type = TOKEN_TYPE_NEGATIVE;
        ResetEmotionState(Tokenizer);
    }
    else if(IsNegationWord(Match))
    {
        Tokenizer->EmotionState = EMOTION_NEGATE;
        Result.Type = TOKEN_TYPE_NEGATION;
    }
    else
    {
        Result.Type = TOKEN_TYPE_IDENTIFIER;
    }

    Result.String = String;
    Result.StringLength = Length;

    Tokenizer->Tokens.push_back(Result);

    printf("Type: %s string: %.*s\n", TokenTypes[Result.Type], Length, String);
}

static void
Tokenize(
    tokenizer *Tokenizer
    )
{
    while(!TokenizerIsEof(Tokenizer))
    {
        SkipWhitespace(Tokenizer);

        if(TokenizerIsEof(Tokenizer))
        {
            return;
        }

        const char *CurrentWord;
        u32 CurrentWordLength;
        {
            u32 StartPosition = Tokenizer->InputPosition;

            while(IsInputCharacterValid(TokenierCurrentCharacter(Tokenizer)) && !TokenizerIsEof(Tokenizer))
            {
                TokenizerAdvance(Tokenizer);
            }

            u32 EndPosition = Tokenizer->InputPosition;
            CurrentWord = Tokenizer->Data + StartPosition;
            CurrentWordLength = EndPosition - StartPosition;

            if(CurrentWordLength == 0)
            {
                printf("Invalid character entered\n");
                exit(1);
            }
        }

        PushToken(Tokenizer, CurrentWord, CurrentWordLength);
    }
}

static s32
SentenceSentiment(
    std::vector<token> Tokens
    )
{
    s32 FinalEmotion = 0;

    static s32 EmotionIndicator[] =
    {
        // TOKEN_TYPE_POSITIVE

        1,

        // TOKEN_TYPE_NEGATIVE
        -1,
        // TOKEN_TYPE_NEGATION
        0,
        // TOKEN_TYPE_IDENTIFIER
        0
    };

    for(token Token: Tokens)
    {
        FinalEmotion += EmotionIndicator[Token.Type];
    }

    return FinalEmotion;
}

const float PI = 3.14159265358979323846f;

void
DrawCircle(
    float CenterX,
    float CenterY,
    float Radius,
    int Segments
    )
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(CenterX, CenterY);

    for(int i = 0; i <= Segments; i++)
    {
        float Angle = i * 2.0f * PI / Segments;
        float X = CenterX + Radius * cos(Angle);
        float Y = CenterY + Radius * sin(Angle);
        glVertex2f(X, Y);
    }
    glEnd();
}

void
DrawArc(
    float CenterX,
    float CenterY,
    float Radius,
    float StartAngle,
    float EndAngle,
    int Segments
    )
{
    glBegin(GL_LINE_STRIP);

    for(int i = 0;
        i <= Segments;
        i++)
    {
        float Angle = StartAngle + (EndAngle - StartAngle) * i / Segments;
        float Rad = Angle * PI / 180.0f;
        float X = CenterX + Radius * cos(Rad);
        float Y = CenterY + Radius * sin(Rad);
        glVertex2f(X, Y);
    }
    glEnd();
}

static void
DisplayEmotion(
    s32 Expression
    )
{
    if(!glfwInit())
    {
        return;
    }

    GLFWwindow* Window = glfwCreateWindow(800, 600, "Cagan Korkmaz Project", NULL, NULL);
    if(!Window)
    {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(Window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return;
    }

    int Width, Height;
    glfwGetFramebufferSize(Window, &Width, &Height);
    glViewport(0, 0, Width, Height);
    glClearColor(0.9f, 0.9f, 1.0f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float Aspect = (float)Width / (float)Height;
    if(Aspect > 1.0f)
    {
        glOrtho(-Aspect, Aspect, -1.0f, 1.0f, -1.0f, 1.0f);
    }
    else
    {
        glOrtho(-1.0f, 1.0f, -1.0f / Aspect, 1.0f / Aspect, -1.0f, 1.0f);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glLineWidth(3.0f);

    while(!glfwWindowShouldClose(Window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glColor3f(1.0f, 1.0f, 0.0f);
        DrawCircle(0.0f, 0.0f, 0.5f, 50);

        glColor3f(0.0f, 0.0f, 0.0f);
        DrawCircle(-0.2f, 0.1f, 0.1f, 30);
        DrawCircle(0.2f, 0.1f, 0.1f, 30);


        glColor3f(0.0f, 0.0f, 0.0f);

        if(Expression > 0)
        {
            DrawArc(0.0f, -0.1f, 0.3f, 200.0f, 340.0f, 30);
        }
        else if(Expression < 0)
        {
            DrawArc(0.0f, -0.35f, 0.3f, 20.0f, 160.0f, 30);
        }
        else
        {
            glBegin(GL_LINES);
            glVertex2f(-0.25f, -0.15f);
            glVertex2f(0.25f, -0.15f);
            glEnd();
        }


        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    glfwTerminate();
}

int
main(
    int ArgumentCount,
    char **Arguments
    )
{

    std::string Paragraph;

    while(1)
    {
        std::getline(std::cin, Paragraph);

        tokenizer Tokenizer = InitializeTokenizer(Paragraph.c_str());

        Tokenize(&Tokenizer);
        s32 Expression = SentenceSentiment(Tokenizer.Tokens);

        DisplayEmotion(Expression);
    }

    return 0;
}
