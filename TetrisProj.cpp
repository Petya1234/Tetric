#include <iostream>
#include <string>
#include <Windows.h>
#include <thread>
#include <vector>
using namespace std;
wstring tetromino[7];
int nFieldWidth = 12;
int nFieldHeight = 18;

int nScreenWidth = 80;
int nScreenHeight = 30;
unsigned char* pField = nullptr;


int Rotate(int px,int py,int r)
{
    switch (r % 4)
    {
    case 0: return py*4 + px; // 0 degrees;
    case 1: return 12 + py - (px * 4); // 90 degrees
    case 2: return 15 - (py * 4) - px; // 180 degrees;
    case 3: return 3 - py + (px*4); // 270 degrees;
    }
    return 0;
}

bool DoesPieceFit(int nTetromino,int nRotation,int nPosX,int nPosY)
{
    for (int px = 0; px < 4; px++)
    {
        for (int py = 0; py < 4; py++)
        {
            // get index into piece
            int pi = Rotate(px, py, nRotation); 

            // get index into field
            int fi = (nPosY + py)*nFieldWidth + (nPosX + px);

            if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
            {
                if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
                {
                    if (tetromino[nTetromino][pi] == L'X' && pField[fi] != 0)
                    {
                        return false; // first hit
                    }
                }
            }
        }
    }

    return true;
}
int main()
{
    // creating assets
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");

    tetromino[1].append(L"..X.");
    tetromino[1].append(L".XX.");
    tetromino[1].append(L".X..");
    tetromino[1].append(L"....");

    tetromino[2].append(L".X.");
    tetromino[2].append(L".XX.");
    tetromino[2].append(L"..X.");
    tetromino[2].append(L"....");

    tetromino[3].append(L"....");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L"....");

    tetromino[4].append(L"..X.");
    tetromino[4].append(L".XX.");
    tetromino[4].append(L"..X.");
    tetromino[4].append(L"....");

    tetromino[5].append(L"....");
    tetromino[5].append(L".XX.");
    tetromino[5].append(L"..X.");
    tetromino[5].append(L"..X.");

    tetromino[6].append(L".XX.");
    tetromino[6].append(L".X..");
    tetromino[6].append(L".X..");
    tetromino[6].append(L"....");

    pField = new unsigned char[nFieldHeight*nFieldWidth]; // creating board
    for (int x = 0; x < nFieldWidth; x++) // board boundary
    {
        for (int y = 0; y < nFieldHeight; y++)
        {
            pField[y*nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;
        }
    }
    wchar_t* screen = new wchar_t[nScreenWidth*nScreenHeight];
    for(int i = 0;i < nScreenWidth*nScreenHeight; i++) screen[i] = L' ';
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    //game logic stuff
    int nCurrentPiece = 0;
    int nCurrentRotation = 0;
    int nCurrentX = nFieldWidth/2;
    int nCurrentY = 0;

    int nSpeed = 20;
    int nSpeedCounter = 0;
    bool bForceDown = false;
    int nPieceCount = 0;
    int nScore = 0;

    bool bGameOver = false;
    bool bKey[4];
    bool bRotateHold = false;

    vector<int> vLines;
    while (!bGameOver)
    {
        // game timing ==================
        this_thread::sleep_for(50ms);
        nSpeedCounter++;
        bForceDown = (nSpeedCounter == nSpeed);
        // input =====================
        for (int k = 0; k < 4; k++)
        {
            bKey[k]=(0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;
        }
        // game logic ==================
        if (bKey[0] && DoesPieceFit(nCurrentPiece,nCurrentRotation,nCurrentX + 1,nCurrentY))
        {
             nCurrentX++;
        }
        if (bKey[1] && DoesPieceFit(nCurrentPiece,nCurrentRotation,nCurrentX - 1,nCurrentY))
        {
             nCurrentX--;
        }
        if (bKey[2] && DoesPieceFit(nCurrentPiece,nCurrentRotation,nCurrentX,nCurrentY + 1))
        {
            nCurrentY++;
        }
        if (bKey[3])
        {
            nCurrentRotation += (!bRotateHold && DoesPieceFit(nCurrentPiece,nCurrentRotation + 1,nCurrentX,nCurrentY)) ? 1 : 0;
            bRotateHold = true;
        }
        else
        {
            bRotateHold = false;
        }

        if (bForceDown)
        {
            if (DoesPieceFit(nCurrentPiece,nCurrentRotation,nCurrentX,nCurrentY + 1))
            {
                nCurrentY++; 
            }
            else
            {
                // lock the current piece
                for (int px = 0; px < 4; px++)
                {
                    for (int py = 0; py < 4; py++)
                    {
                        if (tetromino[nCurrentPiece][Rotate(px,py,nCurrentRotation)] == L'X')
                        {
                            pField[(nCurrentY + py)*nFieldWidth + (nCurrentX + px)] = nCurrentPiece+1;
                        }
                    }
                }

                nPieceCount++;
                if (!(nPieceCount % 10))
                {
                    if(nSpeed >= 10) nSpeed--;
                }
                // check if we got any lines
                for (int py = 0; py < 4; py++)
                {
                    if (nCurrentY + py < nFieldHeight - 1)
                    {
                        bool bLine = true;
                        for (int px = 1; px < nFieldWidth - 1; px++)
                        {
                            bLine &= (pField[(nCurrentY + py)*nFieldWidth + px]) != 0;
                        }
                        if (bLine)
                        {
                            for (int px = 1; px < nFieldWidth - 1; px++)
                            {
                                pField[(nCurrentY + py)*nFieldWidth + px] = 8;
                            }
                            vLines.push_back(nCurrentY + py);
                        }
                    }
                }
                nScore += 25;
                if (!vLines.empty())
                {
                    nScore += (1 << vLines.size()) * 100;
                }

                // choose next piece

                nCurrentPiece = rand() % 7;
                nCurrentRotation = 0;
                nCurrentX = nFieldWidth / 2;
                nCurrentY = 0;

                // if piece doesnt fit

                bGameOver = !DoesPieceFit(nCurrentPiece,nCurrentRotation,nCurrentX,nCurrentY);
            }
            nSpeedCounter = 0;
        }
        // draw field 
        for (int x = 0; x < nFieldWidth; x++)
        {
            for (int y = 0; y < nFieldHeight; y++)
            {
                screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]]; 
            }
        }
        // draw current piece
        for (int px = 0; px < 4; px++)
        {
            for (int py = 0; py < 4; py++)
            {
                if (tetromino[nCurrentPiece][Rotate(px,py,nCurrentRotation)] == L'X')
                {
                    screen[(nCurrentY + py + 2)*nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;
                }
            }
        }

        //draw score 
        swprintf(&screen[2*nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);

        if (!vLines.empty())
        {
            // display frame
            WriteConsoleOutputCharacter(hConsole,screen,nScreenWidth * nScreenHeight,{0,0},&dwBytesWritten);
            this_thread::sleep_for(400ms); // delay a bit

            for (auto& v : vLines)
            {
                for (int px = 1; px < nFieldWidth - 1; px++)
                {
                    for (int py = v; py > 0; py--)
                    {
                        pField[py*nFieldWidth + px] = pField[(py - 1)*nFieldWidth + px];
                    }
                    pField[px] = 0;
                }
            }
            vLines.clear();
        }
        // display frame 
        WriteConsoleOutputCharacter(hConsole,screen,nScreenWidth * nScreenHeight,{0,0},&dwBytesWritten); 
    }

    CloseHandle(hConsole);
    cout << "Game Over\nScore: " << nScore << "\n";
    system("pause");
    return 0;
}
