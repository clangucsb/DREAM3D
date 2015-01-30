/* ============================================================================
 * Copyright (c) 2014 DREAM3D Consortium
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
 * Neither the names of any of the DREAM3D Consortium contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *  This code was partially written under United States Air Force Contract number
 *                              FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "StitchImages.h"

#include "itkMaskedFFTNormalizedCorrelationImageFilter.h"

#include "ItkBridge.h"

#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkPasteImageFilter.h"


#include "ImageProcessing/ImageProcessingHelpers.hpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StitchImages::StitchImages() :
  AbstractFilter(),
  m_AttributeMatrixName(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_StitchedCoordinatesArrayPath(DREAM3D::Defaults::VolumeDataContainerName, "", ""),
  m_StitchedCoordinates(NULL),
  m_StitchedImagesArrayName(""),
  m_StitchedImageArray(NULL),
  m_StitchedAttributeMatrixName("Stitched Images Attribute Matrix")


{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
StitchImages::~StitchImages()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StitchImages::setupFilterParameters()
{
  FilterParameterVector parameters;

  parameters.push_back(FilterParameter::New("Attribute Matrix Name", "AttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getAttributeMatrixName(), false, ""));
  parameters.push_back(FilterParameter::New("Stitched Coordinates", "StitchedCoordinatesArrayPath", FilterParameterWidgetType::DataArraySelectionWidget, getStitchedCoordinatesArrayPath(), false, ""));
  parameters.push_back(FilterParameter::New("Stitched Image Array Name", "StitchedImagesArrayName", FilterParameterWidgetType::StringWidget, getStitchedImagesArrayName(), false, ""));
  parameters.push_back(FilterParameter::New("Stitched Image Attribute Matrix", "StitchedAttributeMatrixName", FilterParameterWidgetType::StringWidget, getStitchedAttributeMatrixName(), true, ""));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StitchImages::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setAttributeMatrixName(reader->readDataArrayPath("AttributeMatrixName", getAttributeMatrixName()));
  setStitchedCoordinatesArrayPath(reader->readDataArrayPath("StitchedCoordinatesArrayPath", getStitchedCoordinatesArrayPath()));
  setStitchedImagesArrayName(reader->readString("StitchedImagesArrayName", getStitchedImagesArrayName()));
  setStitchedAttributeMatrixName(reader->readString("StitchedAttributeMatrix", getStitchedAttributeMatrixName()));
  reader->closeFilterGroup();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int StitchImages::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(AttributeMatrixName)
  DREAM3D_FILTER_WRITE_PARAMETER(StitchedCoordinatesArrayPath)
  DREAM3D_FILTER_WRITE_PARAMETER(StitchedImagesArrayName)
  DREAM3D_FILTER_WRITE_PARAMETER(StitchedAttributeMatrixName)

  writer->closeFilterGroup();
  return ++index;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StitchImages::dataCheck()
{
  setErrorCondition(0);

  DataArrayPath tempPath;


  AttributeMatrix::Pointer am = getDataContainerArray()->getAttributeMatrix(m_AttributeMatrixName);

  if (am.get() == NULL)
  {
      setErrorCondition(-76000);
      notifyErrorMessage(getHumanLabel(), "The attribute matrix has not been selected properly", -76000);
      return;
  }

 QList<QString> names = am->getAttributeArrayNames();


  QVector<size_t> dims(1, 1);


  UInt8ArrayType::Pointer imagePtr = UInt8ArrayType::NullPointer();
  IDataArray::Pointer iDataArray = IDataArray::NullPointer();

  for(int i = 0; i < names.size(); i++)
  {
      tempPath.update(getAttributeMatrixName().getDataContainerName(), getAttributeMatrixName().getAttributeMatrixName(), names[i]);
      iDataArray = getDataContainerArray()->getExistingPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(this, tempPath);

      imagePtr = boost::dynamic_pointer_cast<DataArray<uint8_t> >(iDataArray);
      if(NULL == imagePtr)
      {
          setErrorCondition(-76001);
          notifyErrorMessage(getHumanLabel(), "The data was not found", -76001);
      }
  }

  dims[0] = 2;
  m_StitchedCoordinatesPtr = getDataContainerArray()->getPrereqArrayFromPath<DataArray<float>, AbstractFilter>(this, getStitchedCoordinatesArrayPath(), dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_StitchedCoordinatesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_StitchedCoordinates = m_StitchedCoordinatesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  VolumeDataContainer* m = getDataContainerArray()->getPrereqDataContainer<VolumeDataContainer, AbstractFilter>(this, getStitchedCoordinatesArrayPath().getDataContainerName(), false);
  if(getErrorCondition() < 0 || NULL == m) { return; }

  QVector<size_t> tDims(1, 0);
  AttributeMatrix::Pointer stitchedAttMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getStitchedAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::Cell);
  if(getErrorCondition() < 0){ return; }
  dims[0] = 1;

  tempPath.update(getStitchedCoordinatesArrayPath().getDataContainerName(), getStitchedAttributeMatrixName(), getStitchedImagesArrayName() );
  m_StitchedImageArrayPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<ImageProcessing::DefaultPixelType>, AbstractFilter, ImageProcessing::DefaultPixelType>(this, tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_StitchedImageArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_StitchedImageArray = m_StitchedImageArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StitchImages::preflight()
{
  // These are the REQUIRED lines of CODE to make sure the filter behaves correctly
  setInPreflight(true); // Set the fact that we are preflighting.
  emit preflightAboutToExecute(); // Emit this signal so that other widgets can do one file update
  emit updateFilterParameters(this); // Emit this signal to have the widgets push their values down to the filter
  dataCheck(); // Run our DataCheck to make sure everthing is setup correctly
  emit preflightExecuted(); // We are done preflighting this filter
  setInPreflight(false); // Inform the system this filter is NOT in preflight mode anymore.
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StitchImages::execute()
{

  int err = 0;
  int xval = 0;
  int yval = 0;
  // typically run your dataCheck function to make sure you can get that far and all your variables are initialized
  dataCheck();
  // Check to make sure you made it through the data check. Errors would have been reported already so if something
  // happens to fail in the dataCheck() then we simply return
  if(getErrorCondition() < 0) { return; }
  setErrorCondition(0);

  /* If some error occurs this code snippet can report the error up the call chain*/
  if (err < 0)
  {
      QString ss = QObject::tr("Error Importing a Zeiss AxioVision file set.");
      setErrorCondition(-90000);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      return;
  }

  float minx = 1000000.0f;
  float maxx = 0.0f;
  float miny = 1000000.0f;
  float maxy = 0.0f;
  float value;

  AttributeMatrix::Pointer am = getDataContainerArray()->getAttributeMatrix(m_AttributeMatrixName);


  QList<QString> names = am->getAttributeArrayNames();


  UInt8ArrayType::Pointer imagePtr = UInt8ArrayType::NullPointer();
  IDataArray::Pointer iDataArray = IDataArray::NullPointer();
  uint8_t* image = NULL;

  DataArrayPath tempPath;




  // getting the fist data container just to get the dimensions of each image.
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getAttributeMatrixName().getDataContainerName());


  QVector<size_t> udims;
  udims = am->getTupleDimensions();

#if (CMP_SIZEOF_SIZE_T == 4)
  typedef int32_t DimType;
#else
  typedef int64_t DimType;
#endif
  DimType dims[3] =
  {
    static_cast<DimType>(udims[0]),
    static_cast<DimType>(udims[1]),
    static_cast<DimType>(udims[2]),
  };

  for (size_t i = 0; i < names.size(); i++)
  {
    value = m_StitchedCoordinates[2*i];
    if(value > maxx) { maxx = value; }
    if(value < minx) { minx = value; }

    value = m_StitchedCoordinates[2*i+1];
    if(value > maxy) { maxy = value; }
    if(value < miny) { miny = value; }
  }


//    ImageProcessing::UInt8ImageType* image2 = ImageProcessing::UInt8ImageType::New();
    ImageProcessing::UInt8ImageType::Pointer image2 = ImageProcessing::UInt8ImageType::New();
    ImageProcessing::UInt8ImageType::RegionType region;
    ImageProcessing::UInt8ImageType::IndexType start;
    start[0] = 0;
    start[1] = 0;
    start[2] = 0;

    ImageProcessing::UInt8ImageType::SizeType size;
    unsigned int NumRows = udims[0] + abs(int(maxx)) + abs(int(minx));
    unsigned int NumCols = udims[1] + abs(int(maxy)) + abs(int(miny));
    size[0] = NumRows;
    size[1] = NumCols;
    size[2] = 1;

    region.SetSize(size);
    region.SetIndex(start);


    image2->SetRegions(region);
    image2->Allocate();

    typedef itk::PasteImageFilter <ImageProcessing::UInt8ImageType, ImageProcessing::UInt8ImageType > PasteImageFilterType;
    PasteImageFilterType::Pointer pasteFilter = PasteImageFilterType::New ();

    typedef itk::ImageFileWriter< ImageProcessing::UInt8ImageType > WriterType;
    WriterType::Pointer writer = WriterType::New();

    ImageProcessing::UInt8ImageType::IndexType destinationIndex;

    QVector<size_t> tDims(3);
    tDims[0] = NumRows;
    tDims[1] = NumCols;
    tDims[2] = 1;

    m->getAttributeMatrix(getStitchedAttributeMatrixName())->resizeAttributeArrays(tDims);







// run through all the data containers (images)
  for(size_t i = 0; i < names.size(); i++)
  {
      tempPath.update(getAttributeMatrixName().getDataContainerName(), getAttributeMatrixName().getAttributeMatrixName(), names[i]);
      iDataArray = getDataContainerArray()->getExistingPrereqArrayFromPath<DataArray<uint8_t>, AbstractFilter>(this, tempPath);

      imagePtr = boost::dynamic_pointer_cast<DataArray<uint8_t> >(iDataArray);

      if(NULL != imagePtr.get())
      {
          image = imagePtr->getPointer(0);
          ImageProcessing::ImportUInt8FilterType::Pointer importFilter = ITKUtilitiesType::Dream3DtoITKImportFilter<ImageProcessing::DefaultPixelType>(m, getAttributeMatrixName().getAttributeMatrixName(), image);
          ImageProcessing::UInt8ImageType* currentImage = importFilter->GetOutput();


          destinationIndex[0] = m_StitchedCoordinates[2*i] + abs(int(minx));
          destinationIndex[1] = m_StitchedCoordinates[2*i + 1] + abs(int(miny));;
          destinationIndex[2] = 0;

          pasteFilter->InPlaceOn();
          pasteFilter->SetSourceImage(currentImage);
          pasteFilter->SetDestinationImage(image2);
          pasteFilter->SetSourceRegion(currentImage->GetLargestPossibleRegion());
          pasteFilter->SetDestinationIndex(destinationIndex);
          pasteFilter->Update();
          image2 = pasteFilter->GetOutput();
          image2->DisconnectPipeline();
          ITKUtilitiesType::SetITKFilterOutput(pasteFilter->GetOutput(), m_StitchedImageArrayPtr.lock());
          pasteFilter->Update();



      }



  }

#if 0
  writer->SetFileName( "/Users/megnashah/Desktop/testImage.tiff");
  writer->SetInput( image2 );
  writer->Update();
#endif








  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer StitchImages::newFilterInstance(bool copyFilterParameters)
{
  StitchImages::Pointer filter = StitchImages::New();
  if(true == copyFilterParameters)
  {
    copyFilterParameterInstanceVariables(filter.get());
  }
  return filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString StitchImages::getCompiledLibraryName()
{return ImageProcessing::ImageProcessingBaseName;}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString StitchImages::getGroupName()
{return "ImageProcessing";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString StitchImages::getSubGroupName()
{return "Misc";}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString StitchImages::getHumanLabel()
{return "Stitch Images";}

