/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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


#ifndef _LosAlamosFFTReader_H_
#define _LosAlamosFFTReader_H_

#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Common/AbstractFilter.h"

/**
 * @class LosAlamosFFTReader LosAlamosFFTReader.h DREAM3DLib/IO/LosAlamosFFTReader.h
 * @brief
 * @author wlenthe
 * @date Sep 6, 2013
 * @version 1.0
 */
class DREAM3DLib_EXPORT LosAlamosFFTReader : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(LosAlamosFFTReader)
    DREAM3D_STATIC_NEW_MACRO(LosAlamosFFTReader)
    DREAM3D_TYPE_MACRO_SUPER(LosAlamosFFTReader, AbstractFilter)

    virtual ~LosAlamosFFTReader();

    /* Input Parameters */
    DREAM3D_INSTANCE_STRING_PROPERTY(FieldsFile)
    DREAM3D_INSTANCE_STRING_PROPERTY(RStatsFile)

    //created values
    DREAM3D_INSTANCE_STRING_PROPERTY(Stress11ArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(Stress22ArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(Stress33ArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(Stress23ArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(Stress31ArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(Stress12ArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(Strain11ArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(Strain22ArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(Strain33ArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(Strain23ArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(Strain31ArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(Strain12ArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(StressArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(StrainArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(EEDArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(MaxPrincipalStressArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(MinPrincipalStressArrayName)

    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::IOFilters; }
    virtual const std::string getSubGroupName() { return DREAM3D::FilterSubGroups::InputFilters; }
    virtual const std::string getHumanLabel() { return "Read Los Alamos FFT Output"; }

    virtual void setupFilterParameters();
    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);
    virtual void execute();
    virtual void preflight();

  protected:
    LosAlamosFFTReader();

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

  private:
    float* m_Stress11;
    float* m_Stress22;
    float* m_Stress33;
    float* m_Stress23;
    float* m_Stress31;
    float* m_Stress12;
    float* m_Strain11;
    float* m_Strain22;
    float* m_Strain33;
    float* m_Strain23;
    float* m_Strain31;
    float* m_Strain12;
    float* m_Stress;
    float* m_Strain;
    float* m_EED;
    float* m_MaxPrincipalStress;
    float* m_MinPrincipalStress;

    float sciToF(char*);

    LosAlamosFFTReader(const LosAlamosFFTReader&); //Not Implemented
    void operator=(const LosAlamosFFTReader&); //Not Implemented

};

#endif //_FieldInfoReader_h_

