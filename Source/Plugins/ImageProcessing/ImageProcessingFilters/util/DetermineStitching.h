/* ============================================================================
 * Copyright (c) 2014 Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson, BlueQuartz Software nor the names of
 * its contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
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
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _DetermineStitching_H_
#define _DetermineStitching_H_

#include <vector>

#include <QtCore/QString>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "ImageProcessing/ImageProcessingConstants.h"


/**
 * @brief The DetermineStitching class
 */
class DREAM3DLib_EXPORT DetermineStitching
{
  public:

    virtual ~DetermineStitching();

    /**
   * @brief FindGlobalOrigins
   * @param totalPoints
   * @param udims
   * @param sampleOrigin
   * @param voxelResolution
   * @param dataArrayList
   * @param xGlobCoordsList
   * @param yGlobCoordsList
   * @param xTileList
   * @param yTileList
   * @param obs
   * @return
   */
    static FloatArrayType::Pointer FindGlobalOrigins(size_t totalPoints,
                                                     QVector<size_t> udims,
                                                     float sampleOrigin[],
                                                     float voxelResolution[],
                                                     QVector<ImageProcessing::DefaultPixelType *> dataArrayList,
                                                     QVector<float> xGlobCoordsList,
                                                     QVector<float> yGlobCoordsList,
                                                     QVector<qint32> xTileList,
                                                     QVector<qint32> yTileList,
                                                     AbstractFilter *filter = NULL);

    /**
   * @brief ReturnIndexForCombOrder
   * @param xTileList
   * @param yTileList
   * @param numXtiles
   * @param numYtiles
   * @return
   */
    static QVector<size_t> ReturnIndexForCombOrder(QVector<qint32> xTileList, QVector<qint32> yTileList, size_t numXtiles, size_t numYtiles);

    /**
   * @brief CropAndCrossCorrelate
   * @param cropSpecsIm1Im2
   * @param currentImage
   * @param fixedImage
   * @return
   */
    static std::vector<float> CropAndCrossCorrelate(std::vector<float> cropSpecsIm1Im2, ImageProcessing::UInt8ImageType* currentImage, ImageProcessing::UInt8ImageType* fixedImage);

  protected:
    DetermineStitching();

  private:
    DetermineStitching(const DetermineStitching&); // Copy Constructor Not Implemented
    void operator=(const DetermineStitching&); // Operator '=' Not Implemented
};


#endif /* _DetermineStitching_H_ */
