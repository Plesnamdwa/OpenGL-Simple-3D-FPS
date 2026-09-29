#include <windows.h>
#include <mmsystem.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// =========================================================================
// KONFIGURASI GAME & WINDOW
// =========================================================================
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 750;

// =========================================================================
// SISTEM AUDIO PROSEDURAL (WINMM)
// =========================================================================
enum SoundType {
  SND_SHOOT,
  SND_RELOAD,
  SND_EMPTY,
  SND_ZOMBIE_GROAN,
  SND_ZOMBIE_HIT,
  SND_PLAYER_HURT,
  SND_FLASHLIGHT,
  SND_KEY_PICKUP,
  SND_UNLOCK_PADLOCK,
  SND_DOOR_OPEN
};

void playProceduralSound(SoundType type) {
  const int sampleRate = 22050;
  int numSamples = 0;
  std::vector<short> buffer;

  if (type == SND_SHOOT) {
    numSamples = (int)(sampleRate * 0.28f);
    buffer.resize(numSamples);
    for (int i = 0; i < numSamples; i++) {
      float t = (float)i / sampleRate;
      float env = expf(-t * 18.0f);
      float noise = ((float)rand() / RAND_MAX * 2.0f - 1.0f);
      float lowFreq = sinf(2.0f * M_PI * (180.0f - t * 400.0f) * t);
      float sample = (noise * 0.65f + lowFreq * 0.35f) * env * 32000.0f;
      buffer[i] = (short)(sample > 32767.0f
                              ? 32767.0f
                              : (sample < -32767.0f ? -32767.0f : sample));
    }
  } else if (type == SND_RELOAD) {
    numSamples = (int)(sampleRate * 0.65f);
    buffer.resize(numSamples);
    for (int i = 0; i < numSamples; i++) {
      float t = (float)i / sampleRate;
      float env1 = expf(-fmodf(t, 0.30f) * 45.0f);
      float freq = (t < 0.30f) ? 1200.0f : 850.0f;
      float sample = sinf(2.0f * M_PI * freq * t) * env1 * 20000.0f;
      buffer[i] = (short)sample;
    }
  } else if (type == SND_EMPTY) {
    numSamples = (int)(sampleRate * 0.08f);
    buffer.resize(numSamples);
    for (int i = 0; i < numSamples; i++) {
      float t = (float)i / sampleRate;
      float env = expf(-t * 60.0f);
      float sample = sinf(2.0f * M_PI * 2200.0f * t) * env * 24000.0f;
      buffer[i] = (short)sample;
    }
  } else if (type == SND_ZOMBIE_GROAN) {
    numSamples = (int)(sampleRate * 0.70f);
    buffer.resize(numSamples);
    for (int i = 0; i < numSamples; i++) {
      float t = (float)i / sampleRate;
      float env = sinf(t / 0.70f * M_PI);
      float lfo = sinf(2.0f * M_PI * 5.0f * t);
      float freq = 85.0f + lfo * 25.0f;
      float noise = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * 0.3f;
      float sample = (sinf(2.0f * M_PI * freq * t) + noise) * env * 22000.0f;
      buffer[i] = (short)sample;
    }
  } else if (type == SND_ZOMBIE_HIT) {
    numSamples = (int)(sampleRate * 0.15f);
    buffer.resize(numSamples);
    for (int i = 0; i < numSamples; i++) {
      float t = (float)i / sampleRate;
      float env = expf(-t * 25.0f);
      float noise = ((float)rand() / RAND_MAX * 2.0f - 1.0f);
      float squelch = sinf(2.0f * M_PI * (450.0f - t * 1200.0f) * t);
      float sample = (noise * 0.7f + squelch * 0.3f) * env * 28000.0f;
      buffer[i] = (short)sample;
    }
  } else if (type == SND_PLAYER_HURT) {
    numSamples = (int)(sampleRate * 0.30f);
    buffer.resize(numSamples);
    for (int i = 0; i < numSamples; i++) {
      float t = (float)i / sampleRate;
      float env = expf(-t * 12.0f);
      float low = sinf(2.0f * M_PI * 90.0f * t);
      float sample = low * env * 30000.0f;
      buffer[i] = (short)sample;
    }
  } else if (type == SND_FLASHLIGHT) {
    numSamples = (int)(sampleRate * 0.05f);
    buffer.resize(numSamples);
    for (int i = 0; i < numSamples; i++) {
      float t = (float)i / sampleRate;
      float env = expf(-t * 80.0f);
      float sample = sinf(2.0f * M_PI * 3200.0f * t) * env * 18000.0f;
      buffer[i] = (short)sample;
    }
  } else if (type == SND_KEY_PICKUP) {
    numSamples = (int)(sampleRate * 0.45f);
    buffer.resize(numSamples);
    for (int i = 0; i < numSamples; i++) {
      float t = (float)i / sampleRate;
      float env = expf(-t * 8.0f);
      float s1 = sinf(2.0f * M_PI * 1046.5f * t);
      float s2 = sinf(2.0f * M_PI * 1318.5f * t);
      float s3 = sinf(2.0f * M_PI * 2093.0f * t);
      float sample = (s1 * 0.4f + s2 * 0.4f + s3 * 0.2f) * env * 26000.0f;
      buffer[i] = (short)sample;
    }
  } else if (type == SND_UNLOCK_PADLOCK) {
    numSamples = (int)(sampleRate * 0.60f);
    buffer.resize(numSamples);
    for (int i = 0; i < numSamples; i++) {
      float t = (float)i / sampleRate;
      float env1 = expf(-t * 30.0f);
      float click = sinf(2.0f * M_PI * 1800.0f * t) * env1;
      float env2 = (t > 0.15f) ? expf(-(t - 0.15f) * 15.0f) : 0.0f;
      float metalClang = sinf(2.0f * M_PI * 520.0f * (t - 0.15f)) * env2;
      float sample = (click * 0.6f + metalClang * 0.7f) * 30000.0f;
      buffer[i] = (short)(sample > 32767.0f
                              ? 32767.0f
                              : (sample < -32767.0f ? -32767.0f : sample));
    }
  } else if (type == SND_DOOR_OPEN) {
    numSamples = (int)(sampleRate * 1.5f);
    buffer.resize(numSamples);
    for (int i = 0; i < numSamples; i++) {
      float t = (float)i / sampleRate;
      float env = sinf(t / 1.5f * M_PI);
      float noise = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * 0.4f;
      float rumble = sinf(2.0f * M_PI * 65.0f * t) * 0.6f;
      float sample = (noise + rumble) * env * 28000.0f;
      buffer[i] = (short)sample;
    }
  }

  if (buffer.empty())
    return;

  int dataSize = numSamples * sizeof(short);
  int totalSize = 44 + dataSize;
  std::vector<char> wav(totalSize);

  memcpy(&wav[0], "RIFF", 4);
  int chunkSize = totalSize - 8;
  memcpy(&wav[4], &chunkSize, 4);
  memcpy(&wav[8], "WAVEfmt ", 8);
  int subChunk1Size = 16;
  short audioFormat = 1;
  short numChannels = 1;
  int sRate = sampleRate;
  int byteRate = sampleRate * sizeof(short);
  short blockAlign = sizeof(short);
  short bitsPerSample = 16;
  memcpy(&wav[16], &subChunk1Size, 4);
  memcpy(&wav[20], &audioFormat, 2);
  memcpy(&wav[22], &numChannels, 2);
  memcpy(&wav[24], &sRate, 4);
  memcpy(&wav[28], &byteRate, 4);
  memcpy(&wav[32], &blockAlign, 2);
  memcpy(&wav[34], &bitsPerSample, 2);
  memcpy(&wav[36], "data", 4);
  memcpy(&wav[40], &dataSize, 4);
  memcpy(&wav[44], buffer.data(), dataSize);

  PlaySoundA((LPCSTR)wav.data(), NULL, SND_MEMORY | SND_ASYNC);
}

// =========================================================================
// SISTEM VECTOR FONT RENDERER UNTUK HUD (CRISP & CLEAN ALPHANUMERIC)
// =========================================================================

void drawChar(char c, float x, float y, float size) {
  c = (char)toupper(c);
  float w = size * 0.65f;
  float h = size;
  float lX = x;
  float rX = x + w;
  float topY = y;
  float botY = y + h;
  float midY = y + h * 0.5f;

  glBegin(GL_LINES);
  switch (c) {
  case 'A':
    glVertex2f(lX, botY);
    glVertex2f(lX, topY);
    glVertex2f(lX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, botY);
    glVertex2f(lX, midY);
    glVertex2f(rX, midY);
    break;
  case 'B':
    glVertex2f(lX, topY);
    glVertex2f(lX, botY);
    glVertex2f(lX, topY);
    glVertex2f(rX - 2, topY);
    glVertex2f(rX - 2, topY);
    glVertex2f(rX, topY + 3);
    glVertex2f(rX, topY + 3);
    glVertex2f(rX, midY - 1);
    glVertex2f(rX, midY - 1);
    glVertex2f(lX, midY);
    glVertex2f(lX, midY);
    glVertex2f(rX, midY + 1);
    glVertex2f(rX, midY + 1);
    glVertex2f(rX, botY - 3);
    glVertex2f(rX, botY - 3);
    glVertex2f(rX - 2, botY);
    glVertex2f(rX - 2, botY);
    glVertex2f(lX, botY);
    break;
  case 'C':
    glVertex2f(rX, topY);
    glVertex2f(lX, topY);
    glVertex2f(lX, topY);
    glVertex2f(lX, botY);
    glVertex2f(lX, botY);
    glVertex2f(rX, botY);
    break;
  case 'D':
    glVertex2f(lX, topY);
    glVertex2f(lX, botY);
    glVertex2f(lX, topY);
    glVertex2f(rX - 3, topY);
    glVertex2f(rX - 3, topY);
    glVertex2f(rX, topY + 4);
    glVertex2f(rX, topY + 4);
    glVertex2f(rX, botY - 4);
    glVertex2f(rX, botY - 4);
    glVertex2f(rX - 3, botY);
    glVertex2f(rX - 3, botY);
    glVertex2f(lX, botY);
    break;
  case 'E':
    glVertex2f(rX, topY);
    glVertex2f(lX, topY);
    glVertex2f(lX, topY);
    glVertex2f(lX, botY);
    glVertex2f(lX, botY);
    glVertex2f(rX, botY);
    glVertex2f(lX, midY);
    glVertex2f(rX - 2, midY);
    break;
  case 'F':
    glVertex2f(rX, topY);
    glVertex2f(lX, topY);
    glVertex2f(lX, topY);
    glVertex2f(lX, botY);
    glVertex2f(lX, midY);
    glVertex2f(rX - 2, midY);
    break;
  case 'G':
    glVertex2f(rX, topY);
    glVertex2f(lX, topY);
    glVertex2f(lX, topY);
    glVertex2f(lX, botY);
    glVertex2f(lX, botY);
    glVertex2f(rX, botY);
    glVertex2f(rX, botY);
    glVertex2f(rX, midY);
    glVertex2f(rX, midY);
    glVertex2f(lX + w * 0.4f, midY);
    break;
  case 'H':
    glVertex2f(lX, topY);
    glVertex2f(lX, botY);
    glVertex2f(rX, topY);
    glVertex2f(rX, botY);
    glVertex2f(lX, midY);
    glVertex2f(rX, midY);
    break;
  case 'I':
    glVertex2f(lX, topY);
    glVertex2f(rX, topY);
    glVertex2f(lX + w * 0.5f, topY);
    glVertex2f(lX + w * 0.5f, botY);
    glVertex2f(lX, botY);
    glVertex2f(rX, botY);
    break;
  case 'J':
    glVertex2f(lX, topY);
    glVertex2f(rX, topY);
    glVertex2f(lX + w * 0.7f, topY);
    glVertex2f(lX + w * 0.7f, botY - 3);
    glVertex2f(lX + w * 0.7f, botY - 3);
    glVertex2f(lX + w * 0.4f, botY);
    glVertex2f(lX + w * 0.4f, botY);
    glVertex2f(lX, botY - 3);
    break;
  case 'K':
    glVertex2f(lX, topY);
    glVertex2f(lX, botY);
    glVertex2f(rX, topY);
    glVertex2f(lX, midY);
    glVertex2f(lX, midY);
    glVertex2f(rX, botY);
    break;
  case 'L':
    glVertex2f(lX, topY);
    glVertex2f(lX, botY);
    glVertex2f(lX, botY);
    glVertex2f(rX, botY);
    break;
  case 'M':
    glVertex2f(lX, botY);
    glVertex2f(lX, topY);
    glVertex2f(lX, topY);
    glVertex2f(lX + w * 0.5f, midY);
    glVertex2f(lX + w * 0.5f, midY);
    glVertex2f(rX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, botY);
    break;
  case 'N':
    glVertex2f(lX, botY);
    glVertex2f(lX, topY);
    glVertex2f(lX, topY);
    glVertex2f(rX, botY);
    glVertex2f(rX, botY);
    glVertex2f(rX, topY);
    break;
  case 'O':
    glVertex2f(lX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, botY);
    glVertex2f(rX, botY);
    glVertex2f(lX, botY);
    glVertex2f(lX, botY);
    glVertex2f(lX, topY);
    break;
  case 'P':
    glVertex2f(lX, botY);
    glVertex2f(lX, topY);
    glVertex2f(lX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, midY);
    glVertex2f(rX, midY);
    glVertex2f(lX, midY);
    break;
  case 'Q':
    glVertex2f(lX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, botY);
    glVertex2f(rX, botY);
    glVertex2f(lX, botY);
    glVertex2f(lX, botY);
    glVertex2f(lX, topY);
    glVertex2f(lX + w * 0.4f, botY - h * 0.3f);
    glVertex2f(rX + 2, botY + 2);
    break;
  case 'R':
    glVertex2f(lX, botY);
    glVertex2f(lX, topY);
    glVertex2f(lX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, midY);
    glVertex2f(rX, midY);
    glVertex2f(lX, midY);
    glVertex2f(lX + w * 0.5f, midY);
    glVertex2f(rX, botY);
    break;
  case 'S':
    glVertex2f(rX, topY);
    glVertex2f(lX, topY);
    glVertex2f(lX, topY);
    glVertex2f(lX, midY);
    glVertex2f(lX, midY);
    glVertex2f(rX, midY);
    glVertex2f(rX, midY);
    glVertex2f(rX, botY);
    glVertex2f(rX, botY);
    glVertex2f(lX, botY);
    break;
  case 'T':
    glVertex2f(lX, topY);
    glVertex2f(rX, topY);
    glVertex2f(lX + w * 0.5f, topY);
    glVertex2f(lX + w * 0.5f, botY);
    break;
  case 'U':
    glVertex2f(lX, topY);
    glVertex2f(lX, botY);
    glVertex2f(lX, botY);
    glVertex2f(rX, botY);
    glVertex2f(rX, botY);
    glVertex2f(rX, topY);
    break;
  case 'V':
    glVertex2f(lX, topY);
    glVertex2f(lX + w * 0.5f, botY);
    glVertex2f(lX + w * 0.5f, botY);
    glVertex2f(rX, topY);
    break;
  case 'W':
    glVertex2f(lX, topY);
    glVertex2f(lX + w * 0.25f, botY);
    glVertex2f(lX + w * 0.25f, botY);
    glVertex2f(lX + w * 0.5f, midY);
    glVertex2f(lX + w * 0.5f, midY);
    glVertex2f(rX - w * 0.25f, botY);
    glVertex2f(rX - w * 0.25f, botY);
    glVertex2f(rX, topY);
    break;
  case 'X':
    glVertex2f(lX, topY);
    glVertex2f(rX, botY);
    glVertex2f(lX, botY);
    glVertex2f(rX, topY);
    break;
  case 'Y':
    glVertex2f(lX, topY);
    glVertex2f(lX + w * 0.5f, midY);
    glVertex2f(rX, topY);
    glVertex2f(lX + w * 0.5f, midY);
    glVertex2f(lX + w * 0.5f, midY);
    glVertex2f(lX + w * 0.5f, botY);
    break;
  case 'Z':
    glVertex2f(lX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, topY);
    glVertex2f(lX, botY);
    glVertex2f(lX, botY);
    glVertex2f(rX, botY);
    break;
  case '0':
    glVertex2f(lX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, botY);
    glVertex2f(rX, botY);
    glVertex2f(lX, botY);
    glVertex2f(lX, botY);
    glVertex2f(lX, topY);
    glVertex2f(lX, botY);
    glVertex2f(rX, topY);
    break;
  case '1':
    glVertex2f(lX, topY + 3);
    glVertex2f(lX + w * 0.5f, topY);
    glVertex2f(lX + w * 0.5f, topY);
    glVertex2f(lX + w * 0.5f, botY);
    glVertex2f(lX, botY);
    glVertex2f(rX, botY);
    break;
  case '2':
    glVertex2f(lX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, midY);
    glVertex2f(rX, midY);
    glVertex2f(lX, midY);
    glVertex2f(lX, midY);
    glVertex2f(lX, botY);
    glVertex2f(lX, botY);
    glVertex2f(rX, botY);
    break;
  case '3':
    glVertex2f(lX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, botY);
    glVertex2f(rX, botY);
    glVertex2f(lX, botY);
    glVertex2f(lX, midY);
    glVertex2f(rX, midY);
    break;
  case '4':
    glVertex2f(lX, topY);
    glVertex2f(lX, midY);
    glVertex2f(lX, midY);
    glVertex2f(rX, midY);
    glVertex2f(rX, topY);
    glVertex2f(rX, botY);
    break;
  case '5':
    glVertex2f(rX, topY);
    glVertex2f(lX, topY);
    glVertex2f(lX, topY);
    glVertex2f(lX, midY);
    glVertex2f(lX, midY);
    glVertex2f(rX, midY);
    glVertex2f(rX, midY);
    glVertex2f(rX, botY);
    glVertex2f(rX, botY);
    glVertex2f(lX, botY);
    break;
  case '6':
    glVertex2f(rX, topY);
    glVertex2f(lX, topY);
    glVertex2f(lX, topY);
    glVertex2f(lX, botY);
    glVertex2f(lX, botY);
    glVertex2f(rX, botY);
    glVertex2f(rX, botY);
    glVertex2f(rX, midY);
    glVertex2f(rX, midY);
    glVertex2f(lX, midY);
    break;
  case '7':
    glVertex2f(lX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, topY);
    glVertex2f(lX + w * 0.2f, botY);
    break;
  case '8':
    glVertex2f(lX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, botY);
    glVertex2f(rX, botY);
    glVertex2f(lX, botY);
    glVertex2f(lX, botY);
    glVertex2f(lX, topY);
    glVertex2f(lX, midY);
    glVertex2f(rX, midY);
    break;
  case '9':
    glVertex2f(rX, botY);
    glVertex2f(rX, topY);
    glVertex2f(rX, topY);
    glVertex2f(lX, topY);
    glVertex2f(lX, topY);
    glVertex2f(lX, midY);
    glVertex2f(lX, midY);
    glVertex2f(rX, midY);
    break;
  case ':':
    glVertex2f(lX + w * 0.5f, topY + 3);
    glVertex2f(lX + w * 0.5f, topY + 5);
    glVertex2f(lX + w * 0.5f, botY - 5);
    glVertex2f(lX + w * 0.5f, botY - 3);
    break;
  case '[':
    glVertex2f(rX, topY);
    glVertex2f(lX, topY);
    glVertex2f(lX, topY);
    glVertex2f(lX, botY);
    glVertex2f(lX, botY);
    glVertex2f(rX, botY);
    break;
  case ']':
    glVertex2f(lX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, topY);
    glVertex2f(rX, botY);
    glVertex2f(rX, botY);
    glVertex2f(lX, botY);
    break;
  case '/':
    glVertex2f(lX, botY);
    glVertex2f(rX, topY);
    break;
  case '!':
    glVertex2f(lX + w * 0.5f, topY);
    glVertex2f(lX + w * 0.5f, botY - 4);
    glVertex2f(lX + w * 0.5f, botY - 1);
    glVertex2f(lX + w * 0.5f, botY);
    break;
  case '-':
    glVertex2f(lX, midY);
    glVertex2f(rX, midY);
    break;
  case '+':
    glVertex2f(lX, midY);
    glVertex2f(rX, midY);
    glVertex2f(lX + w * 0.5f, topY + 2);
    glVertex2f(lX + w * 0.5f, botY - 2);
    break;
  default:
    break;
  }
  glEnd();
}

float measureString(const std::string &str, float size) {
  float curX = 0;
  float charW = size * 0.65f;
  float charSpacing = size * 0.25f;
  for (char c : str) {
    if (c == ' ')
      curX += charW * 0.8f;
    else
      curX += charW + charSpacing;
  }
  return curX;
}

void drawString(float x, float y, const std::string &str, float size, float r,
                float g, float b, float lineWidth = 2.0f) {
  glLineWidth(lineWidth);
  glColor3f(r, g, b);
  float curX = x;
  float charW = size * 0.65f;
  float charSpacing = size * 0.25f;

  for (char c : str) {
    if (c == ' ') {
      curX += charW * 0.8f;
    } else {
      drawChar(c, curX, y, size);
      curX += charW + charSpacing;
    }
  }
}

// =========================================================================
// STRUKTUR DATA: PARTIKEL DARAH & EFEK
// =========================================================================
struct BloodParticle {
  float x, y, z;
  float vx, vy, vz;
  float r, g, b, a;
  float size;
  float life;
};
std::vector<BloodParticle> bloodParticles;

void spawnBlood(float x, float y, float z, int count = 25) {
  for (int i = 0; i < count; i++) {
    BloodParticle p;
    p.x = x;
    p.y = y;
    p.z = z;
    p.vx = ((float)rand() / RAND_MAX - 0.5f) * 3.5f;
    p.vy = ((float)rand() / RAND_MAX * 2.5f) + 0.5f;
    p.vz = ((float)rand() / RAND_MAX - 0.5f) * 3.5f;
    p.r = 0.45f + ((float)rand() / RAND_MAX) * 0.20f;
    p.g = 0.02f;
    p.b = 0.02f;
    p.a = 1.0f;
    p.size = 0.04f + ((float)rand() / RAND_MAX) * 0.05f;
    p.life = 0.8f + ((float)rand() / RAND_MAX) * 0.4f;
    bloodParticles.push_back(p);
  }
}

// =========================================================================
// STRUKTUR DATA: KUNCI & GEMBOK (3 WARNA: MERAH, KUNING, HIJAU)
// =========================================================================
enum KeyColor { KEY_RED = 0, KEY_YELLOW = 1, KEY_GREEN = 2 };

struct KeyItem {
  float x, y, z;
  KeyColor color;
  bool isCollected;
  float rotAngle;
};
std::vector<KeyItem> worldKeys;

struct Padlock {
  float x, y, z;
  KeyColor color;
  bool isUnlocked;
  bool isFalling;
  float fallY;
  float fallVy;
  float rotX, rotZ;
  float shackleOpen;
};
std::vector<Padlock> padlocks;

struct VaultDoor {
  float zPos;
  float slideX;
  bool isOpen;
  bool isOpening;
} vaultDoor;

// =========================================================================
// STRUKTUR DATA: ZOMBIE PEKERJA GUDANG
// =========================================================================
struct Zombie {
  float x, y, z;
  float rotY;
  float hp;
  float maxHp;
  float speed;
  bool isAlive;
  float attackCooldown;
  float groanTimer;
  float deathAnimTimer;
  float legAnim;
};
std::vector<Zombie> zombies;

// =========================================================================
// OBJEK LINGKUNGAN GUDANG (COLLIDERS & CRATES)
// =========================================================================
struct BoxCollider {
  float minX, maxX;
  float minZ, maxZ;
};
std::vector<BoxCollider> colliders;

struct CrateObj {
  float x, y, z;
  float sx, sy, sz;
};
std::vector<CrateObj> crates;

// =========================================================================
// STATE PEMAIN & SENJATA
// =========================================================================
struct Player {
  float x, y, z;
  float yaw, pitch;
  float hp;
  float maxHp;
  int ammo;
  int maxAmmo;
  bool isReloading;
  float reloadTimer;
  float shootCooldown;
  float muzzleFlashTimer;
  float damageVignette;
  bool isAiming;
  float aimInterpolation;
  bool flashlightOn;

  // Inventory Kunci (Merah, Kuning, Hijau)
  bool hasKey[3];
} player;

// Prompt Notifikasi HUD & Timer
std::string hudNotification = "";
float hudNotificationTimer = 0.0f;
bool gameWon = false;
float gameTimer = 0.0f;

std::string formatTime(float t) {
  int totalSec = (int)t;
  int mins = totalSec / 60;
  int secs = totalSec % 60;
  char buf[32];
  sprintf(buf, "%02d:%02d", mins, secs);
  return std::string(buf);
}

// Mouse & Input
bool firstMouse = true;
double lastMouseX = 0.0, lastMouseY = 0.0;
bool keyState[1024];
bool mouseLeftDown = false;
bool mouseRightDown = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// =========================================================================
// INISIALISASI DUNIA GUDANG BESAR, KUNCI, GEMBOK, & ZOMBIE
// =========================================================================
void initWarehouse() {
  colliders.clear();
  crates.clear();
  zombies.clear();
  worldKeys.clear();
  padlocks.clear();

  // Dinding Luar Gudang (X dari -20 s/d 20, Z dari +10 s/d -112)
  colliders.push_back({-22.0f, -19.5f, -114.0f, 12.0f}); // Dinding Barat
  colliders.push_back({19.5f, 22.0f, -114.0f, 12.0f});   // Dinding Timur
  colliders.push_back(
      {-22.0f, 22.0f, 9.5f, 12.0f}); // Dinding Selatan (Pintu Masuk)

  // Dinding Belakang Utara Penuh (Kiri & Kanan Pintu Vault - Tidak Ada Celah
  // Samping!)
  colliders.push_back(
      {-22.0f, -4.5f, -111.0f, -109.5f}); // Tembok Kiri Pintu Akhir
  colliders.push_back(
      {4.5f, 22.0f, -111.0f, -109.5f}); // Tembok Kanan Pintu Akhir

  // Dinding Sekat Ruangan Dalam (Internal Partition Walls)
  // Sekat Ruang 1 (Z = -22): Pintu Tengah (-3.5 sampai 3.5)
  colliders.push_back({-20.0f, -3.5f, -22.8f, -21.2f});
  colliders.push_back({3.5f, 20.0f, -22.8f, -21.2f});

  // Sekat Ruang 2 (Z = -52): Pintu Kiri (-14 sampai -8) & Kanan (8 sampai 14)
  colliders.push_back({-8.0f, 8.0f, -52.8f, -51.2f});
  colliders.push_back({-20.0f, -14.0f, -52.8f, -51.2f});
  colliders.push_back({14.0f, 20.0f, -52.8f, -51.2f});

  // Sekat Ruang 3 (Z = -82): Pintu Tengah (-3.5 sampai 3.5)
  colliders.push_back({-20.0f, -3.5f, -82.8f, -81.2f});
  colliders.push_back({3.5f, 20.0f, -82.8f, -81.2f});

  // Pilar-Pilar Beton Gudang
  for (float px = -12.0f; px <= 12.0f; px += 8.0f) {
    for (float pz = -100.0f; pz <= 0.0f; pz += 10.0f) {
      colliders.push_back({px - 0.6f, px + 0.6f, pz - 0.6f, pz + 0.6f});
    }
  }

  auto addCrate = [](float x, float y, float z, float sx, float sy, float sz) {
    crates.push_back({x, y, z, sx, sy, sz});
    colliders.push_back({x - sx, x + sx, z - sz, z + sz});
  };

  // Peti Zona 1 (Z = 0 s/d -20)
  addCrate(-14.0f, 0.8f, -5.0f, 1.2f, 0.8f, 1.2f);
  addCrate(-14.0f, 2.0f, -5.0f, 0.9f, 0.6f, 0.9f);
  addCrate(-16.0f, 1.0f, -7.0f, 1.0f, 1.0f, 1.0f);
  addCrate(14.0f, 1.2f, -12.0f, 1.4f, 1.2f, 1.4f);
  addCrate(16.0f, 0.8f, -14.0f, 1.2f, 0.8f, 1.2f);

  // Peti Zona 2 (Z = -25 s/d -50) - KUNCI MERAH
  addCrate(-16.0f, 0.9f, -35.0f, 1.5f, 0.9f, 1.5f);
  addCrate(-16.0f, 1.8f, -35.0f, 1.0f, 0.5f, 1.0f);
  addCrate(-13.0f, 1.1f, -38.0f, 1.2f, 1.1f, 1.2f);
  addCrate(6.0f, 1.0f, -32.0f, 1.3f, 1.0f, 1.3f);
  addCrate(14.0f, 1.4f, -42.0f, 1.6f, 1.4f, 1.6f);
  addCrate(14.0f, 2.5f, -42.0f, 1.0f, 0.5f, 1.0f);

  // Peti Zona 3 (Z = -55 s/d -80) - KUNCI KUNING
  addCrate(16.0f, 1.0f, -65.0f, 1.5f, 1.0f, 1.5f);
  addCrate(16.0f, 2.0f, -65.0f, 1.1f, 0.6f, 1.1f);
  addCrate(13.0f, 0.8f, -68.0f, 1.2f, 0.8f, 1.2f);
  addCrate(-8.0f, 1.2f, -62.0f, 1.4f, 1.2f, 1.4f);
  addCrate(-15.0f, 1.3f, -72.0f, 1.5f, 1.3f, 1.5f);

  // Peti Zona 4 (Z = -85 s/d -105) - KUNCI HIJAU
  addCrate(-16.0f, 1.0f, -95.0f, 1.5f, 1.0f, 1.5f);
  addCrate(-13.0f, 1.2f, -98.0f, 1.3f, 1.2f, 1.3f);
  addCrate(14.0f, 1.1f, -92.0f, 1.4f, 1.1f, 1.4f);
  addCrate(0.0f, 0.9f, -96.0f, 1.2f, 0.9f, 1.2f);

  // 3 KUNCI BERWARNA
  worldKeys.push_back({-16.5f, 0.85f, -38.5f, KEY_RED, false, 0.0f});
  worldKeys.push_back({16.5f, 0.85f, -68.5f, KEY_YELLOW, false, 0.0f});
  worldKeys.push_back({-16.5f, 0.85f, -98.5f, KEY_GREEN, false, 0.0f});

  // 3 GEMBOK DI PINTU AKHIR
  padlocks.push_back({-1.3f, 1.8f, -109.4f, KEY_RED, false, false, 1.8f, 0.0f,
                      0.0f, 0.0f, 0.0f});
  padlocks.push_back({0.0f, 1.8f, -109.4f, KEY_YELLOW, false, false, 1.8f, 0.0f,
                      0.0f, 0.0f, 0.0f});
  padlocks.push_back({1.3f, 1.8f, -109.4f, KEY_GREEN, false, false, 1.8f, 0.0f,
                      0.0f, 0.0f, 0.0f});

  vaultDoor.zPos = -110.0f;
  vaultDoor.slideX = 0.0f;
  vaultDoor.isOpen = false;
  vaultDoor.isOpening = false;

  // 28 ZOMBIE PEKERJA
  struct ZPos {
    float x, z;
  };
  ZPos zLocs[] = {
      {-12.0f, -8.0f},   {10.0f, -10.0f},  {-4.0f, -16.0f},  {6.0f, -18.0f},
      {-14.0f, -28.0f},  {-8.0f, -32.0f},  {0.0f, -36.0f},   {12.0f, -30.0f},
      {15.0f, -40.0f},   {-16.0f, -42.0f}, {-5.0f, -46.0f},  {8.0f, -48.0f},
      {-12.0f, -58.0f},  {14.0f, -56.0f},  {-3.0f, -64.0f},  {4.0f, -66.0f},
      {16.0f, -72.0f},   {-15.0f, -74.0f}, {0.0f, -76.0f},   {10.0f, -78.0f},
      {-14.0f, -88.0f},  {12.0f, -86.0f},  {-6.0f, -92.0f},  {6.0f, -94.0f},
      {-16.0f, -102.0f}, {15.0f, -100.0f}, {-3.5f, -105.0f}, {3.5f, -105.0f}};

  for (const auto &zp : zLocs) {
    Zombie z;
    z.x = zp.x;
    z.y = 0.0f;
    z.z = zp.z;
    z.rotY = (float)(rand() % 360);
    z.hp = 100.0f;
    z.maxHp = 100.0f;
    z.speed = 1.35f + ((float)rand() / RAND_MAX) * 0.65f;
    z.isAlive = true;
    z.attackCooldown = 0.0f;
    z.groanTimer = 1.5f + ((float)rand() / RAND_MAX) * 5.0f;
    z.deathAnimTimer = 0.0f;
    z.legAnim = 0.0f;
    zombies.push_back(z);
  }
}

void initPlayer() {
  player.x = 0.0f;
  player.y = 1.65f;
  player.z = 7.5f;
  player.yaw = 0.0f;
  player.pitch = 0.0f;
  player.hp = 100.0f;
  player.maxHp = 100.0f;
  player.ammo = 12;
  player.maxAmmo = 12;
  player.isReloading = false;
  player.reloadTimer = 0.0f;
  player.shootCooldown = 0.0f;
  player.muzzleFlashTimer = 0.0f;
  player.damageVignette = 0.0f;
  player.isAiming = false;
  player.aimInterpolation = 0.0f;
  player.flashlightOn = true;

  player.hasKey[KEY_RED] = false;
  player.hasKey[KEY_YELLOW] = false;
  player.hasKey[KEY_GREEN] = false;

  hudNotification = "FIND 3 KEYS TO UNLOCK THE FINAL VAULT DOOR";
  hudNotificationTimer = 5.0f;
  gameWon = false;
  gameTimer = 0.0f;
}

// =========================================================================
// DETEKSI TABRAKAN (COLLISION DETECTION)
// =========================================================================
bool checkCollision(float newX, float newZ, float radius = 0.35f) {
  for (const auto &col : colliders) {
    if (newX + radius > col.minX && newX - radius < col.maxX &&
        newZ + radius > col.minZ && newZ - radius < col.maxZ) {
      return true;
    }
  }
  if (!vaultDoor.isOpen && vaultDoor.slideX < 2.5f) {
    if (newX + radius > -5.0f && newX - radius < 5.0f &&
        newZ + radius > -110.5f && newZ - radius < -109.5f) {
      return true;
    }
  }
  return false;
}

// =========================================================================
// FUNGSI PENGGAMBARAN GEOMETRI 3D
// =========================================================================

void drawCube(float sx, float sy, float sz) {
  glPushMatrix();
  glScalef(sx, sy, sz);
  glBegin(GL_QUADS);
  glNormal3f(0, 0, 1);
  glVertex3f(-0.5f, -0.5f, 0.5f);
  glVertex3f(0.5f, -0.5f, 0.5f);
  glVertex3f(0.5f, 0.5f, 0.5f);
  glVertex3f(-0.5f, 0.5f, 0.5f);

  glNormal3f(0, 0, -1);
  glVertex3f(0.5f, -0.5f, -0.5f);
  glVertex3f(-0.5f, -0.5f, -0.5f);
  glVertex3f(-0.5f, 0.5f, -0.5f);
  glVertex3f(0.5f, 0.5f, -0.5f);

  glNormal3f(-1, 0, 0);
  glVertex3f(-0.5f, -0.5f, -0.5f);
  glVertex3f(-0.5f, -0.5f, 0.5f);
  glVertex3f(-0.5f, 0.5f, 0.5f);
  glVertex3f(-0.5f, 0.5f, -0.5f);

  glNormal3f(1, 0, 0);
  glVertex3f(0.5f, -0.5f, 0.5f);
  glVertex3f(0.5f, -0.5f, -0.5f);
  glVertex3f(0.5f, 0.5f, -0.5f);
  glVertex3f(0.5f, 0.5f, 0.5f);

  glNormal3f(0, 1, 0);
  glVertex3f(-0.5f, 0.5f, 0.5f);
  glVertex3f(0.5f, 0.5f, 0.5f);
  glVertex3f(0.5f, 0.5f, -0.5f);
  glVertex3f(-0.5f, 0.5f, -0.5f);

  glNormal3f(0, -1, 0);
  glVertex3f(-0.5f, -0.5f, -0.5f);
  glVertex3f(0.5f, -0.5f, -0.5f);
  glVertex3f(0.5f, -0.5f, 0.5f);
  glVertex3f(-0.5f, -0.5f, 0.5f);
  glEnd();
  glPopMatrix();
}

void renderKey3D(const KeyItem &k) {
  if (k.isCollected)
    return;

  glPushMatrix();
  glTranslatef(k.x, k.y + sinf(k.rotAngle * 0.05f) * 0.08f, k.z);
  glRotatef(k.rotAngle, 0, 1, 0);

  if (k.color == KEY_RED)
    glColor3f(0.95f, 0.15f, 0.15f);
  else if (k.color == KEY_YELLOW)
    glColor3f(0.95f, 0.85f, 0.10f);
  else if (k.color == KEY_GREEN)
    glColor3f(0.15f, 0.95f, 0.25f);

  drawCube(0.18f, 0.18f, 0.04f);
  glPushMatrix();
  glTranslatef(0.0f, -0.22f, 0.0f);
  drawCube(0.04f, 0.28f, 0.04f);
  glTranslatef(0.06f, -0.08f, 0.0f);
  drawCube(0.08f, 0.04f, 0.04f);
  glTranslatef(0.0f, 0.07f, 0.0f);
  drawCube(0.06f, 0.04f, 0.04f);
  glPopMatrix();

  glPopMatrix();
}

void renderPadlock3D(const Padlock &p) {
  glPushMatrix();
  glTranslatef(p.x, p.fallY, p.z);
  glRotatef(p.rotX, 1, 0, 0);
  glRotatef(p.rotZ, 0, 0, 1);

  if (p.color == KEY_RED)
    glColor3f(0.90f, 0.15f, 0.15f);
  else if (p.color == KEY_YELLOW)
    glColor3f(0.95f, 0.82f, 0.10f);
  else if (p.color == KEY_GREEN)
    glColor3f(0.15f, 0.90f, 0.25f);

  drawCube(0.24f, 0.28f, 0.10f);

  glPushMatrix();
  glTranslatef(0.0f, -0.04f, 0.055f);
  glColor3f(0.10f, 0.10f, 0.10f);
  drawCube(0.04f, 0.08f, 0.02f);
  glPopMatrix();

  glColor3f(0.80f, 0.82f, 0.85f);
  glPushMatrix();
  glTranslatef(0.0f, 0.18f + p.shackleOpen * 0.12f, 0.0f);
  glRotatef(p.shackleOpen * 45.0f, 0, 1, 0);
  drawCube(0.18f, 0.04f, 0.04f);
  glTranslatef(-0.07f, -0.08f, 0.0f);
  drawCube(0.04f, 0.14f, 0.04f);
  glTranslatef(0.14f, 0.0f, 0.0f);
  drawCube(0.04f, 0.14f, 0.04f);
  glPopMatrix();

  glPopMatrix();
}

void renderWarehouseEnvironment() {
  // 1. Lantai Beton Gudang
  glBegin(GL_QUADS);
  glNormal3f(0, 1, 0);
  for (float x = -20.0f; x < 20.0f; x += 2.0f) {
    for (float z = -112.0f; z < 10.0f; z += 2.0f) {
      bool tileDark = ((int)(abs(x) + abs(z)) % 4 == 0);
      if (tileDark)
        glColor3f(0.22f, 0.22f, 0.25f);
      else
        glColor3f(0.30f, 0.30f, 0.33f);
      glVertex3f(x, 0.0f, z);
      glVertex3f(x + 2.0f, 0.0f, z);
      glVertex3f(x + 2.0f, 0.0f, z + 2.0f);
      glVertex3f(x, 0.0f, z + 2.0f);
    }
  }
  glEnd();

  // 2. Plafon Gudang
  glBegin(GL_QUADS);
  glNormal3f(0, -1, 0);
  glColor3f(0.14f, 0.15f, 0.17f);
  glVertex3f(-20.0f, 6.0f, 10.0f);
  glVertex3f(20.0f, 6.0f, 10.0f);
  glVertex3f(20.0f, 6.0f, -112.0f);
  glVertex3f(-20.0f, 6.0f, -112.0f);
  glEnd();

  // 3. Rangka Baja Atap
  glColor3f(0.12f, 0.14f, 0.18f);
  for (float bz = -106.0f; bz <= 6.0f; bz += 6.0f) {
    glPushMatrix();
    glTranslatef(0.0f, 5.8f, bz);
    drawCube(40.0f, 0.3f, 0.4f);
    glPopMatrix();
  }

  // 4. Dinding Luar Gudang
  // Dinding Barat (-X)
  glBegin(GL_QUADS);
  glNormal3f(1, 0, 0);
  glColor3f(0.34f, 0.35f, 0.38f);
  glVertex3f(-20.0f, 0.0f, 10.0f);
  glVertex3f(-20.0f, 0.0f, -112.0f);
  glVertex3f(-20.0f, 6.0f, -112.0f);
  glVertex3f(-20.0f, 6.0f, 10.0f);
  // Dinding Timur (+X)
  glNormal3f(-1, 0, 0);
  glVertex3f(20.0f, 0.0f, -112.0f);
  glVertex3f(20.0f, 0.0f, 10.0f);
  glVertex3f(20.0f, 6.0f, 10.0f);
  glVertex3f(20.0f, 6.0f, -112.0f);
  // Dinding Selatan (+Z) Pintu Masuk
  glNormal3f(0, 0, -1);
  glVertex3f(-20.0f, 0.0f, 10.0f);
  glVertex3f(-2.5f, 0.0f, 10.0f);
  glVertex3f(-2.5f, 6.0f, 10.0f);
  glVertex3f(-20.0f, 6.0f, 10.0f);

  glVertex3f(2.5f, 0.0f, 10.0f);
  glVertex3f(20.0f, 0.0f, 10.0f);
  glVertex3f(20.0f, 6.0f, 10.0f);
  glVertex3f(2.5f, 6.0f, 10.0f);
  glEnd();

  // 5. DINDING UTARA / BELAKANG (-Z = -110.0f) SOLID PENUH (TIDAK ADA CELAH
  // SAMPING!)
  glBegin(GL_QUADS);
  glNormal3f(0, 0, 1);
  glColor3f(0.32f, 0.34f, 0.36f);
  // Dinding Kiri Pintu Akhir (-20 s/d -4.5)
  glVertex3f(-20.0f, 0.0f, -110.0f);
  glVertex3f(-4.5f, 0.0f, -110.0f);
  glVertex3f(-4.5f, 6.0f, -110.0f);
  glVertex3f(-20.0f, 6.0f, -110.0f);

  // Dinding Kanan Pintu Akhir (4.5 s/d 20)
  glVertex3f(4.5f, 0.0f, -110.0f);
  glVertex3f(20.0f, 0.0f, -110.0f);
  glVertex3f(20.0f, 6.0f, -110.0f);
  glVertex3f(4.5f, 6.0f, -110.0f);

  // Dinding Atas Pintu Akhir (Lintel / Arch -4.5 s/d 4.5)
  glVertex3f(-4.5f, 5.0f, -110.0f);
  glVertex3f(4.5f, 5.0f, -110.0f);
  glVertex3f(4.5f, 6.0f, -110.0f);
  glVertex3f(-4.5f, 6.0f, -110.0f);
  glEnd();

  // Lis Kuning Peringatan Bahaya di Dinding Belakang
  glColor3f(0.85f, 0.70f, 0.10f);
  glPushMatrix();
  glTranslatef(-12.25f, 0.25f, -109.9f);
  drawCube(15.5f, 0.4f, 0.05f);
  glPopMatrix();
  glPushMatrix();
  glTranslatef(12.25f, 0.25f, -109.9f);
  drawCube(15.5f, 0.4f, 0.05f);
  glPopMatrix();

  // 6. Sekat Dinding Dalam
  auto drawWallSegment = [](float x1, float z1, float x2, float z2) {
    float cx = (x1 + x2) * 0.5f;
    float cz = (z1 + z2) * 0.5f;
    float sx = fabsf(x2 - x1) + 0.4f;
    float sz = fabsf(z2 - z1) + 0.4f;
    glPushMatrix();
    glTranslatef(cx, 3.0f, cz);
    glColor3f(0.32f, 0.34f, 0.36f);
    drawCube(sx, 6.0f, sz);
    glTranslatef(0.0f, -2.7f, 0.0f);
    glColor3f(0.85f, 0.70f, 0.10f);
    drawCube(sx + 0.02f, 0.4f, sz + 0.02f);
    glPopMatrix();
  };

  drawWallSegment(-20.0f, -22.0f, -3.5f, -22.0f);
  drawWallSegment(3.5f, -22.0f, 20.0f, -22.0f);

  drawWallSegment(-8.0f, -52.0f, 8.0f, -52.0f);
  drawWallSegment(-20.0f, -52.0f, -14.0f, -52.0f);
  drawWallSegment(14.0f, -52.0f, 20.0f, -52.0f);

  drawWallSegment(-20.0f, -82.0f, -3.5f, -82.0f);
  drawWallSegment(3.5f, -82.0f, 20.0f, -82.0f);

  // 7. Pilar Beton
  for (float px = -12.0f; px <= 12.0f; px += 8.0f) {
    for (float pz = -100.0f; pz <= 0.0f; pz += 10.0f) {
      glPushMatrix();
      glTranslatef(px, 3.0f, pz);
      glColor3f(0.40f, 0.42f, 0.45f);
      drawCube(1.0f, 6.0f, 1.0f);
      glColor3f(0.10f, 0.35f, 0.65f);
      drawCube(1.02f, 0.4f, 1.02f);
      glPopMatrix();
    }
  }

  // 8. Peti Kayu
  for (const auto &c : crates) {
    glPushMatrix();
    glTranslatef(c.x, c.y, c.z);
    glColor3f(0.55f, 0.38f, 0.22f);
    drawCube(c.sx * 2.0f, c.sy * 2.0f, c.sz * 2.0f);
    glColor3f(0.38f, 0.24f, 0.12f);
    drawCube(c.sx * 2.02f, c.sy * 0.15f, c.sz * 2.02f);
    drawCube(c.sx * 0.15f, c.sy * 2.02f, c.sz * 2.02f);
    glPopMatrix();
  }

  // 9. PINTU AKHIR VAULT (Z = -110.0f) DENGAN KUSEN BESI TERTUTUP RAPAT
  // Kusen Baja Pintu Vault
  glPushMatrix();
  glTranslatef(0.0f, 2.5f, -109.95f);
  // Pilar Kusen Kiri
  glPushMatrix();
  glTranslatef(-4.6f, 0.0f, 0.0f);
  glColor3f(0.20f, 0.22f, 0.26f);
  drawCube(0.5f, 5.2f, 0.4f);
  glPopMatrix();
  // Pilar Kusen Kanan
  glPushMatrix();
  glTranslatef(4.6f, 0.0f, 0.0f);
  glColor3f(0.20f, 0.22f, 0.26f);
  drawCube(0.5f, 5.2f, 0.4f);
  glPopMatrix();
  // Palang Kusen Atas
  glPushMatrix();
  glTranslatef(0.0f, 2.5f, 0.0f);
  glColor3f(0.20f, 0.22f, 0.26f);
  drawCube(9.7f, 0.5f, 0.4f);
  glPopMatrix();
  glPopMatrix();

  // Daun Pintu Kiri
  glPushMatrix();
  glTranslatef(-2.25f - vaultDoor.slideX, 2.5f, -110.0f);
  glColor3f(0.28f, 0.32f, 0.38f);
  drawCube(4.5f, 4.8f, 0.25f);
  glColor3f(0.85f, 0.70f, 0.10f);
  drawCube(4.3f, 0.15f, 0.27f);
  glPopMatrix();

  // Daun Pintu Kanan
  glPushMatrix();
  glTranslatef(2.25f + vaultDoor.slideX, 2.5f, -110.0f);
  glColor3f(0.28f, 0.32f, 0.38f);
  drawCube(4.5f, 4.8f, 0.25f);
  glColor3f(0.85f, 0.70f, 0.10f);
  drawCube(4.3f, 0.15f, 0.27f);
  glPopMatrix();

  // Rantai / Palang Besi Penahan Gembok
  if (!vaultDoor.isOpen) {
    glPushMatrix();
    glTranslatef(0.0f, 1.8f, -109.6f);
    glColor3f(0.20f, 0.22f, 0.25f);
    drawCube(3.8f, 0.22f, 0.12f);
    glPopMatrix();
  }

  // Cahaya Luar di Balik Pintu Vault
  if (vaultDoor.isOpen || vaultDoor.slideX > 0.5f) {
    glPushMatrix();
    glTranslatef(0.0f, 3.0f, -114.0f);
    glColor3f(0.95f, 0.98f, 1.0f);
    drawCube(12.0f, 6.0f, 1.5f);
    glPopMatrix();
  }
}

void renderZombie(const Zombie &z) {
  if (!z.isAlive && z.deathAnimTimer >= 1.0f) {
    glPushMatrix();
    glTranslatef(z.x, 0.15f, z.z);
    glRotatef(z.rotY, 0, 1, 0);
    glRotatef(90.0f, 1, 0, 0);
    glColor3f(0.20f, 0.28f, 0.22f);
    drawCube(0.6f, 0.9f, 0.35f);
    glPushMatrix();
    glTranslatef(0.0f, 0.65f, 0.0f);
    glColor3f(0.45f, 0.52f, 0.42f);
    drawCube(0.35f, 0.35f, 0.35f);
    glPopMatrix();
    glPopMatrix();
    return;
  }

  glPushMatrix();
  glTranslatef(z.x, z.y, z.z);
  glRotatef(z.rotY, 0, 1, 0);

  if (!z.isAlive) {
    float fallAngle = z.deathAnimTimer * 90.0f;
    glRotatef(fallAngle, 1, 0, 0);
  }

  float legSwing = sinf(z.legAnim) * 25.0f;
  glPushMatrix();
  glTranslatef(-0.16f, 0.45f, 0.0f);
  if (z.isAlive)
    glRotatef(legSwing, 1, 0, 0);
  glColor3f(0.20f, 0.25f, 0.30f);
  drawCube(0.22f, 0.9f, 0.24f);
  glTranslatef(0.0f, -0.42f, 0.05f);
  glColor3f(0.10f, 0.10f, 0.10f);
  drawCube(0.24f, 0.15f, 0.32f);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.16f, 0.45f, 0.0f);
  if (z.isAlive)
    glRotatef(-legSwing, 1, 0, 0);
  glColor3f(0.20f, 0.25f, 0.30f);
  drawCube(0.22f, 0.9f, 0.24f);
  glTranslatef(0.0f, -0.42f, 0.05f);
  glColor3f(0.10f, 0.10f, 0.10f);
  drawCube(0.24f, 0.15f, 0.32f);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, 1.25f, 0.0f);
  glColor3f(0.12f, 0.28f, 0.48f);
  drawCube(0.55f, 0.70f, 0.32f);
  glPushMatrix();
  glTranslatef(0.05f, -0.05f, 0.17f);
  glColor3f(0.40f, 0.02f, 0.02f);
  drawCube(0.30f, 0.35f, 0.02f);
  glPopMatrix();
  glPopMatrix();

  float armSway = sinf(z.legAnim * 0.8f) * 10.0f;
  glPushMatrix();
  glTranslatef(-0.35f, 1.45f, 0.35f);
  glRotatef(-75.0f + armSway, 1, 0, 0);
  glColor3f(0.45f, 0.52f, 0.42f);
  drawCube(0.16f, 0.65f, 0.16f);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.35f, 1.45f, 0.35f);
  glRotatef(-80.0f - armSway, 1, 0, 0);
  glColor3f(0.45f, 0.52f, 0.42f);
  drawCube(0.16f, 0.65f, 0.16f);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, 1.80f, 0.0f);
  glColor3f(0.48f, 0.56f, 0.45f);
  drawCube(0.36f, 0.38f, 0.36f);

  if (z.isAlive) {
    glColor3f(0.95f, 0.10f, 0.10f);
    glPushMatrix();
    glTranslatef(-0.09f, 0.04f, 0.19f);
    drawCube(0.06f, 0.06f, 0.02f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.09f, 0.04f, 0.19f);
    drawCube(0.06f, 0.06f, 0.02f);
    glPopMatrix();
  }

  glTranslatef(0.0f, 0.20f, 0.0f);
  glColor3f(0.85f, 0.70f, 0.10f);
  drawCube(0.44f, 0.18f, 0.46f);
  glPopMatrix();

  glPopMatrix();
}

void renderFirstPersonPistol() {
  glClear(GL_DEPTH_BUFFER_BIT);
  glDisable(GL_LIGHTING);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  float aspect = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
  float fov = player.isAiming ? 35.0f : 50.0f;
  float fH = tanf(fov / 360.0f * M_PI) * 0.1f;
  float fW = fH * aspect;
  glFrustum(-fW, fW, -fH, fH, 0.1f, 20.0f);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  float hipX = 0.18f, hipY = -0.16f, hipZ = -0.36f;
  float aimX = 0.00f, aimY = -0.095f, aimZ = -0.28f;

  float curX = hipX + (aimX - hipX) * player.aimInterpolation;
  float curY = hipY + (aimY - hipY) * player.aimInterpolation;
  float curZ = hipZ + (aimZ - hipZ) * player.aimInterpolation;

  float recoilOffset = 0.0f;
  float recoilRot = 0.0f;
  if (player.muzzleFlashTimer > 0.0f) {
    recoilOffset = player.muzzleFlashTimer * 0.45f;
    recoilRot = player.muzzleFlashTimer * 120.0f;
  }

  float reloadOffsetY = 0.0f;
  if (player.isReloading) {
    float rProg = player.reloadTimer / 1.5f;
    reloadOffsetY = -sinf(rProg * M_PI) * 0.25f;
  }

  glTranslatef(curX, curY - recoilOffset + reloadOffsetY,
               curZ + recoilOffset * 0.5f);
  glRotatef(recoilRot, 1, 0, 0);

  glColor3f(0.18f, 0.19f, 0.21f);
  glPushMatrix();
  glTranslatef(0.0f, 0.02f, 0.0f);
  drawCube(0.038f, 0.042f, 0.24f);
  glPopMatrix();

  glColor3f(0.08f, 0.08f, 0.09f);
  glPushMatrix();
  glTranslatef(0.0f, 0.025f, -0.125f);
  drawCube(0.020f, 0.020f, 0.03f);
  glPopMatrix();

  glColor3f(0.10f, 0.10f, 0.11f);
  glPushMatrix();
  glTranslatef(0.0f, -0.045f, 0.05f);
  glRotatef(-15.0f, 1, 0, 0);
  drawCube(0.034f, 0.12f, 0.06f);
  glPopMatrix();

  glColor3f(0.15f, 0.15f, 0.16f);
  glPushMatrix();
  glTranslatef(0.0f, -0.02f, 0.00f);
  drawCube(0.015f, 0.04f, 0.06f);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, 0.046f, -0.11f);
  glColor3f(0.10f, 0.95f, 0.20f);
  drawCube(0.006f, 0.012f, 0.008f);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-0.012f, 0.046f, 0.11f);
  glColor3f(0.10f, 0.95f, 0.20f);
  drawCube(0.005f, 0.012f, 0.008f);
  glTranslatef(0.024f, 0.0f, 0.0f);
  drawCube(0.005f, 0.012f, 0.008f);
  glPopMatrix();

  if (player.muzzleFlashTimer > 0.0f) {
    glPushMatrix();
    glTranslatef(0.0f, 0.025f, -0.22f);
    glColor3f(1.0f, 0.90f, 0.30f);
    drawCube(0.08f, 0.08f, 0.12f);
    glColor3f(1.0f, 0.45f, 0.10f);
    drawCube(0.14f, 0.14f, 0.06f);
    glPopMatrix();
  }

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_LIGHTING);
}

// =========================================================================
// RENDER 2D HUD (DARAH, AMUNISI, CROSSHAIR, INVENTORY 3 KUNCI, TEXT HUD)
// =========================================================================

void renderHUD() {
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  // 1. Efek Vignette Darah
  if (player.damageVignette > 0.0f) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.6f, 0.0f, 0.0f, player.damageVignette * 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    glVertex2f(0, WINDOW_HEIGHT);
    glEnd();
    glDisable(GL_BLEND);
  }

  // 2. Kursor / Crosshair Presisi di Tengah Layar
  float cx = WINDOW_WIDTH / 2.0f;
  float cy = WINDOW_HEIGHT / 2.0f;
  float gap = 6.0f - player.aimInterpolation * 3.5f;
  float len = 8.0f - player.aimInterpolation * 3.0f;

  glLineWidth(2.0f);
  glColor3f(0.95f - player.aimInterpolation * 0.15f, 0.95f,
            0.95f - player.aimInterpolation * 0.75f);
  glBegin(GL_LINES);
  glVertex2f(cx - gap - len, cy);
  glVertex2f(cx - gap, cy);
  glVertex2f(cx + gap, cy);
  glVertex2f(cx + gap + len, cy);
  glVertex2f(cx, cy - gap - len);
  glVertex2f(cx, cy - gap);
  glVertex2f(cx, cy + gap);
  glVertex2f(cx, cy + gap + len);
  glEnd();
  glPointSize(3.0f + player.aimInterpolation * 2.0f);
  glBegin(GL_POINTS);
  glVertex2f(cx, cy);
  glEnd();

  // 3. Status Bar Darah di Kiri Atas
  float hpBarX = 35.0f;
  float hpBarY = 35.0f;
  float hpBarW = 220.0f;
  float hpBarH = 22.0f;

  // Background Bar
  glColor3f(0.12f, 0.12f, 0.14f);
  glBegin(GL_QUADS);
  glVertex2f(hpBarX - 4, hpBarY - 4);
  glVertex2f(hpBarX + hpBarW + 4, hpBarY - 4);
  glVertex2f(hpBarX + hpBarW + 4, hpBarY + hpBarH + 4);
  glVertex2f(hpBarX - 4, hpBarY + hpBarH + 4);
  glEnd();

  // Bar Isi Darah
  float hpRatio = player.hp / player.maxHp;
  if (hpRatio < 0.0f)
    hpRatio = 0.0f;
  float curHpW = hpBarW * hpRatio;

  glBegin(GL_QUADS);
  glColor3f(0.9f * (1.0f - hpRatio), 0.85f * hpRatio + 0.1f, 0.15f);
  glVertex2f(hpBarX, hpBarY);
  glVertex2f(hpBarX + curHpW, hpBarY);
  glVertex2f(hpBarX + curHpW, hpBarY + hpBarH);
  glVertex2f(hpBarX, hpBarY + hpBarH);
  glEnd();

  glLineWidth(2.0f);
  glColor3f(0.85f, 0.85f, 0.88f);
  glBegin(GL_LINE_LOOP);
  glVertex2f(hpBarX - 4, hpBarY - 4);
  glVertex2f(hpBarX + hpBarW + 4, hpBarY - 4);
  glVertex2f(hpBarX + hpBarW + 4, hpBarY + hpBarH + 4);
  glVertex2f(hpBarX - 4, hpBarY + hpBarH + 4);
  glEnd();

  // Teks Label Darah
  drawString(hpBarX + 6, hpBarY + 5,
             "HP: " + std::to_string((int)player.hp) + "/100", 12.0f, 0.98f,
             0.98f, 0.98f, 2.0f);

  // Timer di Bawah Bar Darah (00:00)
  float timerY = hpBarY + hpBarH + 8.0f;
  std::string timeStr = "TIME: " + formatTime(gameTimer);
  // Background Badge Timer
  glColor3f(0.08f, 0.08f, 0.10f);
  glBegin(GL_QUADS);
  glVertex2f(hpBarX - 4, timerY - 2);
  glVertex2f(hpBarX + 118, timerY - 2);
  glVertex2f(hpBarX + 118, timerY + 18);
  glVertex2f(hpBarX - 4, timerY + 18);
  glEnd();
  glLineWidth(1.5f);
  glColor3f(0.35f, 0.38f, 0.42f);
  glBegin(GL_LINE_LOOP);
  glVertex2f(hpBarX - 4, timerY - 2);
  glVertex2f(hpBarX + 118, timerY - 2);
  glVertex2f(hpBarX + 118, timerY + 18);
  glVertex2f(hpBarX - 4, timerY + 18);
  glEnd();
  drawString(hpBarX + 4, timerY + 3, timeStr, 12.0f, 0.95f, 0.92f, 0.35f, 2.0f);

  // 4. INVENTORY 3 KOTAK KUNCI DI KANAN ATAS
  float invStartX = WINDOW_WIDTH - 230.0f;
  float invY = 32.0f;
  float slotSize = 52.0f;
  float slotGap = 12.0f;

  // Background & Header Inventory
  glColor3f(0.08f, 0.08f, 0.10f);
  glBegin(GL_QUADS);
  glVertex2f(invStartX - 10, invY - 18);
  glVertex2f(invStartX + (slotSize + slotGap) * 3 + 4, invY - 18);
  glVertex2f(invStartX + (slotSize + slotGap) * 3 + 4, invY + slotSize + 8);
  glVertex2f(invStartX - 10, invY + slotSize + 8);
  glEnd();

  drawString(invStartX - 2, invY - 15, "KEYS INVENTORY", 10.0f, 0.75f, 0.78f,
             0.82f, 1.5f);

  const char *keyNames[3] = {"RED", "YELLOW", "GREEN"};

  for (int k = 0; k < 3; k++) {
    float sx = invStartX + k * (slotSize + slotGap);
    float sy = invY;

    glColor3f(0.14f, 0.15f, 0.17f);
    glBegin(GL_QUADS);
    glVertex2f(sx, sy);
    glVertex2f(sx + slotSize, sy);
    glVertex2f(sx + slotSize, sy + slotSize);
    glVertex2f(sx, sy + slotSize);
    glEnd();

    glLineWidth(2.5f);
    if (player.hasKey[k]) {
      if (k == KEY_RED)
        glColor3f(0.95f, 0.20f, 0.20f);
      else if (k == KEY_YELLOW)
        glColor3f(0.95f, 0.85f, 0.15f);
      else if (k == KEY_GREEN)
        glColor3f(0.20f, 0.95f, 0.30f);
    } else {
      glColor3f(0.32f, 0.33f, 0.35f);
    }
    glBegin(GL_LINE_LOOP);
    glVertex2f(sx, sy);
    glVertex2f(sx + slotSize, sy);
    glVertex2f(sx + slotSize, sy + slotSize);
    glVertex2f(sx, sy + slotSize);
    glEnd();

    if (player.hasKey[k]) {
      float kcx = sx + slotSize * 0.5f;
      float kcy = sy + slotSize * 0.5f - 4;

      if (k == KEY_RED)
        glColor3f(0.95f, 0.20f, 0.20f);
      else if (k == KEY_YELLOW)
        glColor3f(0.95f, 0.85f, 0.15f);
      else if (k == KEY_GREEN)
        glColor3f(0.20f, 0.95f, 0.30f);

      glBegin(GL_LINE_LOOP);
      glVertex2f(kcx - 8, kcy - 10);
      glVertex2f(kcx + 8, kcy - 10);
      glVertex2f(kcx + 8, kcy + 2);
      glVertex2f(kcx - 8, kcy + 2);
      glEnd();
      glBegin(GL_LINES);
      glVertex2f(kcx, kcy + 2);
      glVertex2f(kcx, kcy + 16);
      glVertex2f(kcx, kcy + 10);
      glVertex2f(kcx + 6, kcy + 10);
      glVertex2f(kcx, kcy + 15);
      glVertex2f(kcx + 4, kcy + 15);
      glEnd();

      // Label Warna Kunci
      float tw = measureString(keyNames[k], 8.0f);
      drawString(sx + (slotSize - tw) * 0.5f, sy + slotSize - 10, keyNames[k],
                 8.0f, 0.95f, 0.95f, 0.95f, 1.5f);
    } else {
      std::string slotNum = "[" + std::to_string(k + 1) + "]";
      float tw = measureString(slotNum, 11.0f);
      drawString(sx + (slotSize - tw) * 0.5f, sy + 20.0f, slotNum, 11.0f, 0.40f,
                 0.42f, 0.45f, 1.5f);
    }
  }

  // 5. Panel Jumlah Amunisi di Kiri Bawah
  float ammoX = 35.0f;
  float ammoY = WINDOW_HEIGHT - 55.0f;

  // Teks Label Amunisi (Dinaikkan agar tidak tertutup garis reload)
  if (player.isReloading) {
    drawString(ammoX, ammoY - 30.0f, "RELOADING...", 12.0f, 0.95f, 0.85f, 0.20f,
               2.0f);
  } else {
    drawString(ammoX, ammoY - 22.0f,
               "AMMO: " + std::to_string(player.ammo) + " / " +
                   std::to_string(player.maxAmmo),
               12.0f, 0.85f, 0.88f, 0.92f, 2.0f);
  }

  for (int b = 0; b < player.maxAmmo; b++) {
    float bx = ammoX + b * 18.0f;
    float by = ammoY;
    if (b < player.ammo)
      glColor3f(0.95f, 0.78f, 0.15f);
    else
      glColor3f(0.25f, 0.25f, 0.28f);

    glBegin(GL_QUADS);
    glVertex2f(bx, by);
    glVertex2f(bx + 12.0f, by);
    glVertex2f(bx + 12.0f, by + 26.0f);
    glVertex2f(bx, by + 26.0f);
    glEnd();

    glLineWidth(1.0f);
    glColor3f(0.10f, 0.10f, 0.12f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(bx, by);
    glVertex2f(bx + 12.0f, by);
    glVertex2f(bx + 12.0f, by + 26.0f);
    glVertex2f(bx, by + 26.0f);
    glEnd();
  }

  if (player.isReloading) {
    float rProg = player.reloadTimer / 1.5f;
    glColor3f(0.95f, 0.85f, 0.20f);
    glBegin(GL_QUADS);
    glVertex2f(ammoX, ammoY - 14.0f);
    glVertex2f(ammoX + (216.0f * rProg), ammoY - 14.0f);
    glVertex2f(ammoX + (216.0f * rProg), ammoY - 8.0f);
    glVertex2f(ammoX, ammoY - 8.0f);
    glEnd();
  }

  // 6. Pesan Prompt Interaksi & Notifikasi Teks di Layar
  if (hudNotificationTimer > 0.0f && !hudNotification.empty()) {
    float strW = measureString(hudNotification, 14.0f);
    float pw = strW + 40.0f;
    float ph = 38.0f;
    float px = (WINDOW_WIDTH - pw) / 2.0f;
    float py = WINDOW_HEIGHT - 120.0f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.05f, 0.05f, 0.08f, 0.88f);
    glBegin(GL_QUADS);
    glVertex2f(px, py);
    glVertex2f(px + pw, py);
    glVertex2f(px + pw, py + ph);
    glVertex2f(px, py + ph);
    glEnd();

    glLineWidth(2.0f);
    glColor3f(0.95f, 0.85f, 0.20f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(px, py);
    glVertex2f(px + pw, py);
    glVertex2f(px + pw, py + ph);
    glVertex2f(px, py + ph);
    glEnd();
    glDisable(GL_BLEND);

    drawString(px + 20.0f, py + 12.0f, hudNotification, 14.0f, 0.98f, 0.95f,
               0.30f, 2.0f);
  }

  // 7. Layar Game Over & Victory dengan Teks Lengkap
  if (player.hp <= 0.0f) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.5f, 0.0f, 0.0f, 0.80f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    glVertex2f(0, WINDOW_HEIGHT);
    glEnd();
    glDisable(GL_BLEND);

    float bw = 480.0f, bh = 180.0f;
    float bx = (WINDOW_WIDTH - bw) / 2.0f;
    float by = (WINDOW_HEIGHT - bh) / 2.0f;

    glColor3f(0.08f, 0.08f, 0.10f);
    glBegin(GL_QUADS);
    glVertex2f(bx, by);
    glVertex2f(bx + bw, by);
    glVertex2f(bx + bw, by + bh);
    glVertex2f(bx, by + bh);
    glEnd();

    glColor3f(0.9f, 0.1f, 0.1f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(bx, by);
    glVertex2f(bx + bw, by);
    glVertex2f(bx + bw, by + bh);
    glVertex2f(bx, by + bh);
    glEnd();

    std::string t1 = "YOU DIED";
    std::string t2 = "KILLED BY ZOMBIE WORKERS";
    std::string t3 = "PRESS [R] TO RESTART";

    drawString(bx + (bw - measureString(t1, 32.0f)) * 0.5f, by + 30.0f, t1,
               32.0f, 0.95f, 0.15f, 0.15f, 3.5f);
    drawString(bx + (bw - measureString(t2, 14.0f)) * 0.5f, by + 90.0f, t2,
               14.0f, 0.85f, 0.85f, 0.88f, 2.0f);
    drawString(bx + (bw - measureString(t3, 16.0f)) * 0.5f, by + 130.0f, t3,
               16.0f, 0.95f, 0.85f, 0.20f, 2.5f);
  } else if (gameWon) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.05f, 0.35f, 0.10f, 0.80f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(WINDOW_WIDTH, 0);
    glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
    glVertex2f(0, WINDOW_HEIGHT);
    glEnd();
    glDisable(GL_BLEND);

    float bw = 520.0f, bh = 200.0f;
    float bx = (WINDOW_WIDTH - bw) / 2.0f;
    float by = (WINDOW_HEIGHT - bh) / 2.0f;

    glColor3f(0.06f, 0.10f, 0.06f);
    glBegin(GL_QUADS);
    glVertex2f(bx, by);
    glVertex2f(bx + bw, by);
    glVertex2f(bx + bw, by + bh);
    glVertex2f(bx, by + bh);
    glEnd();

    glColor3f(0.2f, 0.95f, 0.3f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(bx, by);
    glVertex2f(bx + bw, by);
    glVertex2f(bx + bw, by + bh);
    glVertex2f(bx, by + bh);
    glEnd();

    std::string t1 = "ESCAPE SUCCESSFUL!";
    std::string t2 = "ALL 3 VAULT LOCKS UNLOCKED";
    std::string tTime = "ESCAPE TIME: " + formatTime(gameTimer);
    std::string t3 = "PRESS [R] TO PLAY AGAIN";

    drawString(bx + (bw - measureString(t1, 28.0f)) * 0.5f, by + 28.0f, t1,
               28.0f, 0.25f, 0.95f, 0.35f, 3.5f);
    drawString(bx + (bw - measureString(t2, 14.0f)) * 0.5f, by + 80.0f, t2,
               14.0f, 0.85f, 0.92f, 0.88f, 2.0f);
    drawString(bx + (bw - measureString(tTime, 16.0f)) * 0.5f, by + 115.0f,
               tTime, 16.0f, 0.98f, 0.88f, 0.25f, 2.5f);
    drawString(bx + (bw - measureString(t3, 15.0f)) * 0.5f, by + 155.0f, t3,
               15.0f, 0.95f, 0.95f, 0.95f, 2.0f);
  }

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
}

// =========================================================================
// LOGIKA SHOOTING & HITSCAN RAYCAST
// =========================================================================
void shootWeapon() {
  if (player.shootCooldown > 0.0f || player.isReloading || player.hp <= 0.0f ||
      gameWon)
    return;

  if (player.ammo <= 0) {
    playProceduralSound(SND_EMPTY);
    player.isReloading = true;
    player.reloadTimer = 0.0f;
    playProceduralSound(SND_RELOAD);
    return;
  }

  player.ammo--;
  player.shootCooldown = 0.22f;
  player.muzzleFlashTimer = 0.08f;
  playProceduralSound(SND_SHOOT);

  float radYaw = player.yaw * M_PI / 180.0f;
  float radPitch = player.pitch * M_PI / 180.0f;
  float dirX = sinf(radYaw) * cosf(radPitch);
  float dirY = sinf(radPitch);
  float dirZ = -cosf(radYaw) * cosf(radPitch);

  float closestDist = 1000.0f;
  int hitZombieIndex = -1;

  for (size_t i = 0; i < zombies.size(); i++) {
    if (!zombies[i].isAlive)
      continue;

    float toZomX = zombies[i].x - player.x;
    float toZomY = (zombies[i].y + 1.2f) - player.y;
    float toZomZ = zombies[i].z - player.z;

    float proj = toZomX * dirX + toZomY * dirY + toZomZ * dirZ;
    if (proj > 0.5f && proj < 35.0f) {
      float perpX = toZomX - proj * dirX;
      float perpY = toZomY - proj * dirY;
      float perpZ = toZomZ - proj * dirZ;
      float distToRaySq = perpX * perpX + perpY * perpY + perpZ * perpZ;

      if (distToRaySq < 0.35f && proj < closestDist) {
        closestDist = proj;
        hitZombieIndex = (int)i;
      }
    }
  }

  if (hitZombieIndex != -1) {
    Zombie &targetZ = zombies[hitZombieIndex];
    float dmg = player.isAiming ? 65.0f : 45.0f;
    targetZ.hp -= dmg;
    playProceduralSound(SND_ZOMBIE_HIT);

    float hitX = player.x + dirX * closestDist;
    float hitY = player.y + dirY * closestDist;
    float hitZ = player.z + dirZ * closestDist;
    spawnBlood(hitX, hitY, hitZ, 30);

    if (targetZ.hp <= 0.0f) {
      targetZ.isAlive = false;
      targetZ.deathAnimTimer = 0.0f;
    }
  }
}

// =========================================================================
// INTERAKSI PEMAIN (AMBIL KUNCI & BUKA GEMBOK)
// =========================================================================
void interactWorld() {
  for (auto &k : worldKeys) {
    if (k.isCollected)
      continue;
    float dist = sqrtf((k.x - player.x) * (k.x - player.x) +
                       (k.z - player.z) * (k.z - player.z));
    if (dist < 2.2f) {
      k.isCollected = true;
      player.hasKey[k.color] = true;
      playProceduralSound(SND_KEY_PICKUP);
      if (k.color == KEY_RED)
        hudNotification = "OBTAINED RED KEY!";
      else if (k.color == KEY_YELLOW)
        hudNotification = "OBTAINED YELLOW KEY!";
      else if (k.color == KEY_GREEN)
        hudNotification = "OBTAINED GREEN KEY!";
      hudNotificationTimer = 3.5f;
      return;
    }
  }

  for (auto &p : padlocks) {
    if (p.isUnlocked)
      continue;
    float dist = sqrtf((p.x - player.x) * (p.x - player.x) +
                       (p.z - player.z) * (p.z - player.z));
    if (dist < 3.2f) {
      if (player.hasKey[p.color]) {
        p.isUnlocked = true;
        p.isFalling = true;
        p.fallVy = 1.5f;
        player.hasKey[p.color] = false;
        playProceduralSound(SND_UNLOCK_PADLOCK);

        if (p.color == KEY_RED)
          hudNotification = "UNLOCKED RED PADLOCK!";
        else if (p.color == KEY_YELLOW)
          hudNotification = "UNLOCKED YELLOW PADLOCK!";
        else if (p.color == KEY_GREEN)
          hudNotification = "UNLOCKED GREEN PADLOCK!";
        hudNotificationTimer = 3.0f;

        bool allUnlocked = true;
        for (const auto &l : padlocks) {
          if (!l.isUnlocked) {
            allUnlocked = false;
            break;
          }
        }
        if (allUnlocked) {
          vaultDoor.isOpening = true;
          playProceduralSound(SND_DOOR_OPEN);
          hudNotification = "ALL PADLOCKS UNLOCKED! VAULT IS OPENING!";
          hudNotificationTimer = 4.0f;
        }
        return;
      } else {
        if (p.color == KEY_RED)
          hudNotification = "LOCKED! REQUIRES RED KEY";
        else if (p.color == KEY_YELLOW)
          hudNotification = "LOCKED! REQUIRES YELLOW KEY";
        else if (p.color == KEY_GREEN)
          hudNotification = "LOCKED! REQUIRES GREEN KEY";
        hudNotificationTimer = 2.5f;
      }
    }
  }
}

// =========================================================================
// UPDATE LOGIKA GAME TIAP FRAME
// =========================================================================
void updateGame(float dt) {
  if (player.hp <= 0.0f || gameWon) {
    if (keyState[GLFW_KEY_R]) {
      initWarehouse();
      initPlayer();
    }
    return;
  }

  // Update penghitung waktu bermain
  gameTimer += dt;

  if (hudNotificationTimer > 0.0f)
    hudNotificationTimer -= dt;

  if (player.shootCooldown > 0.0f)
    player.shootCooldown -= dt;
  if (player.muzzleFlashTimer > 0.0f)
    player.muzzleFlashTimer -= dt;
  if (player.damageVignette > 0.0f)
    player.damageVignette -= dt * 1.8f;

  if (player.isReloading) {
    player.reloadTimer += dt;
    if (player.reloadTimer >= 1.5f) {
      player.ammo = player.maxAmmo;
      player.isReloading = false;
      player.reloadTimer = 0.0f;
    }
  } else if (keyState[GLFW_KEY_R] && player.ammo < player.maxAmmo) {
    player.isReloading = true;
    player.reloadTimer = 0.0f;
    playProceduralSound(SND_RELOAD);
  }

  player.isAiming = mouseRightDown;
  float targetAim = player.isAiming ? 1.0f : 0.0f;
  player.aimInterpolation += (targetAim - player.aimInterpolation) * 12.0f * dt;

  float radYaw = player.yaw * M_PI / 180.0f;
  float forwardX = sinf(radYaw);
  float forwardZ = -cosf(radYaw);
  float rightX = cosf(radYaw);
  float rightZ = sinf(radYaw);

  float moveSpeed = (keyState[GLFW_KEY_LEFT_SHIFT] ? 4.8f : 3.0f);
  if (player.isAiming)
    moveSpeed *= 0.60f;

  float moveX = 0.0f, moveZ = 0.0f;
  if (keyState[GLFW_KEY_W]) {
    moveX += forwardX;
    moveZ += forwardZ;
  }
  if (keyState[GLFW_KEY_S]) {
    moveX -= forwardX;
    moveZ -= forwardZ;
  }
  if (keyState[GLFW_KEY_A]) {
    moveX -= rightX;
    moveZ -= rightZ;
  }
  if (keyState[GLFW_KEY_D]) {
    moveX += rightX;
    moveZ += rightZ;
  }

  float moveLen = sqrtf(moveX * moveX + moveZ * moveZ);
  if (moveLen > 0.001f) {
    moveX = (moveX / moveLen) * moveSpeed * dt;
    moveZ = (moveZ / moveLen) * moveSpeed * dt;

    if (!checkCollision(player.x + moveX, player.z))
      player.x += moveX;
    if (!checkCollision(player.x, player.z + moveZ))
      player.z += moveZ;
  }

  // Deteksi Otomatis Ambil Kunci & Prompt Interaksi Gembok
  for (auto &k : worldKeys) {
    if (!k.isCollected) {
      k.rotAngle += dt * 75.0f;
      float dist = sqrtf((k.x - player.x) * (k.x - player.x) +
                         (k.z - player.z) * (k.z - player.z));
      if (dist < 1.8f) {
        k.isCollected = true;
        player.hasKey[k.color] = true;
        playProceduralSound(SND_KEY_PICKUP);
        if (k.color == KEY_RED)
          hudNotification = "OBTAINED RED KEY!";
        else if (k.color == KEY_YELLOW)
          hudNotification = "OBTAINED YELLOW KEY!";
        else if (k.color == KEY_GREEN)
          hudNotification = "OBTAINED GREEN KEY!";
        hudNotificationTimer = 3.5f;
      }
    }
  }

  // Deteksi Dekat Gembok untuk Prompt Tombol E
  if (player.z < -106.0f && player.z > -110.5f && fabsf(player.x) < 4.0f &&
      hudNotificationTimer <= 0.0f) {
    for (const auto &p : padlocks) {
      if (!p.isUnlocked) {
        if (player.hasKey[p.color]) {
          hudNotification = "PRESS [E] TO UNLOCK PADLOCK";
          hudNotificationTimer = 0.2f;
          break;
        }
      }
    }
  }

  for (auto &p : padlocks) {
    if (p.isFalling) {
      if (p.shackleOpen < 1.0f)
        p.shackleOpen += dt * 6.0f;
      p.fallVy -= 9.8f * dt;
      p.fallY += p.fallVy * dt;
      p.rotX += dt * 140.0f;
      p.rotZ += dt * 90.0f;
      if (p.fallY <= 0.12f) {
        p.fallY = 0.12f;
        p.fallVy = -p.fallVy * 0.35f;
        if (fabsf(p.fallVy) < 0.2f) {
          p.isFalling = false;
          p.fallVy = 0.0f;
        }
      }
    }
  }

  if (vaultDoor.isOpening) {
    vaultDoor.slideX += dt * 1.8f;
    if (vaultDoor.slideX >= 3.8f) {
      vaultDoor.slideX = 3.8f;
      vaultDoor.isOpen = true;
      vaultDoor.isOpening = false;
    }
  }

  if (vaultDoor.isOpen && player.z < -110.8f) {
    gameWon = true;
  }

  for (size_t i = 0; i < bloodParticles.size();) {
    bloodParticles[i].life -= dt;
    if (bloodParticles[i].life <= 0.0f) {
      bloodParticles.erase(bloodParticles.begin() + i);
    } else {
      bloodParticles[i].x += bloodParticles[i].vx * dt;
      bloodParticles[i].y += bloodParticles[i].vy * dt;
      bloodParticles[i].z += bloodParticles[i].vz * dt;
      bloodParticles[i].vy -= 9.8f * dt;
      if (bloodParticles[i].y < 0.02f) {
        bloodParticles[i].y = 0.02f;
        bloodParticles[i].vx = 0.0f;
        bloodParticles[i].vy = 0.0f;
        bloodParticles[i].vz = 0.0f;
      }
      i++;
    }
  }

  for (auto &z : zombies) {
    if (!z.isAlive) {
      if (z.deathAnimTimer < 1.0f)
        z.deathAnimTimer += dt * 3.0f;
      continue;
    }

    z.groanTimer -= dt;
    if (z.groanTimer <= 0.0f) {
      float distToPlayer = sqrtf((z.x - player.x) * (z.x - player.x) +
                                 (z.z - player.z) * (z.z - player.z));
      if (distToPlayer < 20.0f) {
        playProceduralSound(SND_ZOMBIE_GROAN);
      }
      z.groanTimer = 3.5f + ((float)rand() / RAND_MAX) * 5.0f;
    }

    float dx = player.x - z.x;
    float dz = player.z - z.z;
    float dist = sqrtf(dx * dx + dz * dz);

    z.rotY = atan2f(dx, dz) * 180.0f / M_PI;

    if (dist > 1.15f && dist < 24.0f) {
      float nX = (dx / dist) * z.speed * dt;
      float nZ = (dz / dist) * z.speed * dt;

      if (!checkCollision(z.x + nX, z.z, 0.4f))
        z.x += nX;
      if (!checkCollision(z.x, z.z + nZ, 0.4f))
        z.z += nZ;

      z.legAnim += dt * 8.0f;
    }

    if (z.attackCooldown > 0.0f)
      z.attackCooldown -= dt;
    if (dist <= 1.35f && z.attackCooldown <= 0.0f) {
      player.hp -= 20.0f;
      player.damageVignette = 1.0f;
      playProceduralSound(SND_PLAYER_HURT);
      z.attackCooldown = 1.2f;
    }
  }
}

// =========================================================================
// RENDER SCENE 3D UTAMA
// =========================================================================
void renderScene() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.04f, 0.04f, 0.06f, 1.0f);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  float aspect = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
  float fov = player.isAiming ? 45.0f : 65.0f;
  float fH = tanf(fov / 360.0f * M_PI) * 0.1f;
  float fW = fH * aspect;
  glFrustum(-fW, fW, -fH, fH, 0.1f, 100.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glEnable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

  GLfloat ambientLight[] = {0.18f, 0.18f, 0.22f, 1.0f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

  glEnable(GL_FOG);
  GLfloat fogColor[] = {0.04f, 0.04f, 0.06f, 1.0f};
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogi(GL_FOG_MODE, GL_LINEAR);
  glFogf(GL_FOG_START, 6.0f);
  glFogf(GL_FOG_END, 34.0f);

  if (player.flashlightOn) {
    glEnable(GL_LIGHT0);
    GLfloat lightPosEye[] = {0.0f, -0.1f, 0.0f, 1.0f};
    GLfloat spotDirEye[] = {0.0f, 0.0f, -1.0f};

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosEye);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDirEye);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 38.0f);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 4.0f);

    GLfloat spotDiffuse[] = {1.4f, 1.4f, 1.3f, 1.0f};
    GLfloat spotSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, spotDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spotSpecular);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.015f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.001f);
  } else {
    glDisable(GL_LIGHT0);
  }

  if (player.muzzleFlashTimer > 0.0f) {
    glEnable(GL_LIGHT1);
    GLfloat flashPosEye[] = {0.15f, -0.1f, -0.5f, 1.0f};
    GLfloat flashColor[] = {2.0f, 1.6f, 0.6f, 1.0f};
    glLightfv(GL_LIGHT1, GL_POSITION, flashPosEye);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, flashColor);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.08f);
  } else {
    glDisable(GL_LIGHT1);
  }

  glRotatef(-player.pitch, 1.0f, 0.0f, 0.0f);
  glRotatef(player.yaw, 0.0f, 1.0f, 0.0f);
  glTranslatef(-player.x, -player.y, -player.z);

  renderWarehouseEnvironment();

  for (const auto &k : worldKeys) {
    renderKey3D(k);
  }

  for (const auto &p : padlocks) {
    renderPadlock3D(p);
  }

  for (const auto &z : zombies) {
    renderZombie(z);
  }

  glDisable(GL_LIGHTING);
  for (const auto &p : bloodParticles) {
    glPushMatrix();
    glTranslatef(p.x, p.y, p.z);
    glColor4f(p.r, p.g, p.b, p.a);
    drawCube(p.size, p.size, p.size);
    glPopMatrix();
  }
  glEnable(GL_LIGHTING);

  renderFirstPersonPistol();

  renderHUD();
}

// =========================================================================
// CALLBACK INPUT GLFW
// =========================================================================
void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  if (key >= 0 && key < 1024) {
    if (action == GLFW_PRESS)
      keyState[key] = true;
    if (action == GLFW_RELEASE)
      keyState[key] = false;
  }

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  if (key == GLFW_KEY_E && action == GLFW_PRESS) {
    interactWorld();
  }

  if (key == GLFW_KEY_F && action == GLFW_PRESS) {
    player.flashlightOn = !player.flashlightOn;
    playProceduralSound(SND_FLASHLIGHT);
  }
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      mouseLeftDown = true;
      shootWeapon();
    } else if (action == GLFW_RELEASE) {
      mouseLeftDown = false;
    }
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    if (action == GLFW_PRESS) {
      mouseRightDown = true;
    } else if (action == GLFW_RELEASE) {
      mouseRightDown = false;
    }
  }
}

void cursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
  if (firstMouse) {
    lastMouseX = xpos;
    lastMouseY = ypos;
    firstMouse = false;
  }

  float xoffset = (float)(xpos - lastMouseX);
  float yoffset = (float)(lastMouseY - ypos);
  lastMouseX = xpos;
  lastMouseY = ypos;

  float sensitivity = player.isAiming ? 0.065f : 0.11f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  player.yaw += xoffset;
  player.pitch += yoffset;

  if (player.pitch > 85.0f)
    player.pitch = 85.0f;
  if (player.pitch < -85.0f)
    player.pitch = -85.0f;
}

// =========================================================================
// ENTRY POINT UTAMA
// =========================================================================
int main() {
  srand((unsigned int)time(NULL));

  if (!glfwInit()) {
    std::cerr << "Gagal menginisialisasi GLFW!" << std::endl;
    return -1;
  }

  GLFWwindow *window = glfwCreateWindow(
      WINDOW_WIDTH, WINDOW_HEIGHT,
      "3D Horror FPS: Outbreak at SPPG Warehouse - 3 Keys Vault", NULL, NULL);
  if (!window) {
    std::cerr << "Gagal membuat window GLFW!" << std::endl;
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glfwSetKeyCallback(window, keyCallback);
  glfwSetMouseButtonCallback(window, mouseButtonCallback);
  glfwSetCursorPosCallback(window, cursorPosCallback);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glShadeModel(GL_SMOOTH);

  initWarehouse();
  initPlayer();

  lastFrame = (float)glfwGetTime();

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if (deltaTime > 0.1f)
      deltaTime = 0.1f;

    updateGame(deltaTime);
    renderScene();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}