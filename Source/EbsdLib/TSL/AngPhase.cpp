/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  This code was written under United States Air Force Contract number
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */





#include "AngPhase.h"
#include <string.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngPhase::AngPhase() :
m_PhaseIndex(-1)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AngPhase::~AngPhase()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parsePhase(char* value, size_t start, size_t length)
{
  if (value[start] == ':')
  {
    ++start;
  } // move past the ":" character
  QByteArray buf(&(value[start]), strlen(value) - start);
  bool ok = false;
  m_PhaseIndex = buf.toInt(&ok, 10);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parseMaterialName(char* value, size_t start, size_t length)
{
  if (value[start] == ':')
  {
    ++start;
  } // move past the ":" character
  while (value[start] == ' ' || value[start] == '\t')
  {
    ++start;
  }

  size_t len = strlen(value);
  // Strip off training new line and carriage returns
  if (value[len-1] == '\r' || value[len-1] == '\n')
  {
    len--;
  }
  if (value[len-1] == '\r' || value[len-1] == '\n')
  {
    len--;
  }
  QByteArray data(&(value[start]), len - start);
  m_MaterialName = QString(data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parseFormula(char* value, size_t start, size_t length)
{
  if (value[start] == ':')
  {
    ++start;
  } // move past the ":" character
  while (value[start] == ' ' || value[start] == '\t')
  {
    ++start;
  }

  size_t len = strlen(value);
  // Strip off training new line and carriage returns
  if (value[len-1] == '\r' || value[len-1] == '\n')
  {
    len--;
  }
  if (value[len-1] == '\r' || value[len-1] == '\n')
  {
    len--;
  }
  QByteArray data(&(value[start]), len - start);
  m_Formula = QString(data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parseInfo(char* value, size_t start, size_t length)
{
  if (value[start] == ':')
  {
    ++start;
  } // move past the ":" character
  while (value[start] == ' ' || value[start] == '\t')
  {
    ++start;
  }

  size_t len = strlen(value);
  // Strip off training new line and carriage returns
  if (value[len-1] == '\r' || value[len-1] == '\n')
  {
    len--;
  }
  if (value[len-1] == '\r' || value[len-1] == '\n')
  {
    len--;
  }
  QByteArray data(&(value[start]), len - start);
  m_Info = QString(data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parseSymmetry(char* value, size_t start, size_t length)
{
  if (value[start] == ':')
  {
    ++start;
  } // move past the ":" character
  QByteArray data(&(value[start]), strlen(value) - start);
  bool ok = false;
  m_Symmetry = data.toUInt(&ok, 10);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parseLatticeConstants(char* value, size_t start, size_t length)
{
  m_LatticeConstants.clear();

  if (value[start] == ':')
  {
    ++start;
  } // move past the ":" character
  while (value[start] == ' ' || value[start] == '\t')
  {
    ++start;
  }

  size_t len = strlen(value);
  // Strip off training new line and carriage returns
  if (value[len-1] == '\r' || value[len-1] == '\n')
  {
    len--;
  }
  if (value[len-1] == '\r' || value[len-1] == '\n')
  {
    len--;
  }
  QByteArray buf(&(value[start]), len - start);
  buf = buf.simplified(); // Trims beginning and ending white space and ensures only a single space between elements
  QList<QByteArray> tokens = buf.split(' '); // Splits into tokens using the SPACE delimiter
  bool ok = false;
  m_LatticeConstants.push_back(tokens[0].toFloat(&ok));
  m_LatticeConstants.push_back(tokens[1].toFloat(&ok));
  m_LatticeConstants.push_back(tokens[2].toFloat(&ok));
  m_LatticeConstants.push_back(tokens[3].toFloat(&ok));
  m_LatticeConstants.push_back(tokens[4].toFloat(&ok));
  m_LatticeConstants.push_back(tokens[5].toFloat(&ok));

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parseNumberFamilies(char* value, size_t start, size_t length)
{
  if (value[start] == ':')
  {
    ++start;
  } // move past the ":" character
  QByteArray data(&(value[start]), strlen(value) - start);
  bool ok = false;
  m_NumberFamilies = data.toInt(&ok, 10);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parseHKLFamilies(char* value, size_t start, size_t length)
{
  HKLFamily::Pointer family = HKLFamily::New();
  if (value[start] == ':')
  {
    ++start;
  } // move past the ":" character
  QByteArray buf(&(value[start]), strlen(value) - start);
  buf = buf.simplified(); // Trims beginning and ending white space and ensures only a single space between elements
  QList<QByteArray> tokens = buf.split(' '); // Splits into tokens using the SPACE delimiter
  bool ok = false;
  family->h = tokens[0].toInt(&ok, 10);
  family->k = tokens[1].toInt(&ok, 10);
  family->l = tokens[2].toInt(&ok, 10);
  family->s1 = tokens[3].toInt(&ok, 10);
  family->diffractionIntensity = tokens[4].toFloat(&ok);
  if (tokens.size() > 5)
  {
    family->s2 = tokens[5].toInt(&ok, 10);
  }
  if (family->s1 > 1) { family->s1 = 1; }
  if (family->s2 > 1) { family->s2 = 1; }
  m_HKLFamilies.push_back(family);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::parseCategories(char* value, size_t start, size_t length)
{
  m_Categories.clear();

  if (value[start] == ':')
  {
    ++start;
  } // move past the ":" character
  while (value[start] == ' ' || value[start] == '\t')
  {
    ++start;
  }

  size_t len = strlen(value);
  // Strip off training new line and carriage returns
  if (value[len-1] == '\r' || value[len-1] == '\n')
  {
    len--;
  }
  if (value[len-1] == '\r' || value[len-1] == '\n')
  {
    len--;
  }
  QByteArray buf(&(value[start]), len - start);
  buf = buf.simplified(); // Trims beginning and ending white space and ensures only a single space between elements
  QList<QByteArray> tokens = buf.split(' '); // Splits into tokens using the SPACE delimiter
  bool ok = false;
  foreach(QByteArray cat, tokens)
  {
    m_Categories.push_back(cat.toInt(&ok, 10));
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AngPhase::printSelf(QTextStream &stream)
{
  stream << Ebsd::Ang::Phase << ": " << m_PhaseIndex << QString("\n");
  stream << Ebsd::Ang::MaterialName << ": " << m_MaterialName << QString("\n");
  stream << Ebsd::Ang::Formula << ": " << m_Formula << QString("\n");
  stream << Ebsd::Ang::Info << ": " << m_Info << QString("\n");
  stream << Ebsd::Ang::Symmetry << ": " << m_Symmetry << QString("\n");

  stream << Ebsd::Ang::LatticeConstants;
  for (QVector<float>::iterator iter = m_LatticeConstants.begin(); iter != m_LatticeConstants.end(); ++iter )
  {
    stream << " " << *iter;
  }
  stream << QString("\n");

  stream << Ebsd::Ang::NumberFamilies << ": " << m_NumberFamilies << QString("\n");


  for (QVector<HKLFamily::Pointer>::iterator iter = m_HKLFamilies.begin(); iter != m_HKLFamilies.end(); ++iter )
  {
   (*iter)->printSelf(stream);
  }


  stream << Ebsd::Ang::Categories;
  for (QVector<int>::iterator iter = m_Categories.begin(); iter != m_Categories.end(); ++iter )
  {
    stream << " " << *iter;
  }
  stream << QString("\n");

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int AngPhase::determineCrystalStructure()
{
  uint32_t symmetry = getSymmetry();
  unsigned int crystal_structure = Ebsd::CrystalStructure::UnknownCrystalStructure;

  switch(symmetry)
  {
    case Ebsd::Ang::PhaseSymmetry::Cubic:
      crystal_structure = Ebsd::CrystalStructure::Cubic_High;
      break;
    case Ebsd::Ang::PhaseSymmetry::Tetrahedral:
      crystal_structure = Ebsd::CrystalStructure::Cubic_Low;
      break;
    case Ebsd::Ang::PhaseSymmetry::DiTetragonal:
      crystal_structure = Ebsd::CrystalStructure::Tetragonal_High;
      break;
    case Ebsd::Ang::PhaseSymmetry::Tetragonal:
      crystal_structure = Ebsd::CrystalStructure::Tetragonal_Low;
      break;
    case Ebsd::Ang::PhaseSymmetry::Orthorhombic:
      crystal_structure = Ebsd::CrystalStructure::OrthoRhombic;
      break;
    case Ebsd::Ang::PhaseSymmetry::Monoclinic_c:
    case Ebsd::Ang::PhaseSymmetry::Monoclinic_b:
    case Ebsd::Ang::PhaseSymmetry::Monoclinic_a:
      crystal_structure = Ebsd::CrystalStructure::Monoclinic;
      break;
    case Ebsd::Ang::PhaseSymmetry::Triclinic:
      crystal_structure = Ebsd::CrystalStructure::Triclinic;
      break;
    case Ebsd::Ang::PhaseSymmetry::DiHexagonal:
      crystal_structure = Ebsd::CrystalStructure::Hexagonal_High;
      break;
    case Ebsd::Ang::PhaseSymmetry::Hexagonal:
      crystal_structure = Ebsd::CrystalStructure::Hexagonal_Low;
      break;
    case Ebsd::Ang::PhaseSymmetry::DiTrigonal:
      crystal_structure = Ebsd::CrystalStructure::Trigonal_High;
      break;
    case Ebsd::Ang::PhaseSymmetry::Trigonal:
      crystal_structure = Ebsd::CrystalStructure::Trigonal_Low;
      break;


    default:
      crystal_structure = Ebsd::CrystalStructure::UnknownCrystalStructure;

  }
  return crystal_structure;
}
