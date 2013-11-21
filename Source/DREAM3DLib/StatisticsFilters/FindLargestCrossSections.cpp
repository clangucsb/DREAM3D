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

#include "FindLargestCrossSections.h"

#include "DREAM3DLib/Math/DREAM3DMath.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/GenericFilters/FindFeaturePhases.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindLargestCrossSections::FindLargestCrossSections() :
  AbstractFilter(),
  m_DataContainerName(DREAM3D::HDF5::VolumeDataContainerName),
  m_FeatureIdsArrayName(DREAM3D::CellData::FeatureIds),
  m_LargestCrossSectionsArrayName(DREAM3D::FeatureData::LargestCrossSections),
  m_Plane(0),
  m_FeatureIds(NULL),
  m_LargestCrossSections(NULL)
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FindLargestCrossSections::~FindLargestCrossSections()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLargestCrossSections::setupFilterParameters()
{
  FilterParameterVector parameters;
  {
    ChoiceFilterParameter::Pointer option = ChoiceFilterParameter::New();
    option->setHumanLabel("Plane of Interest");
    option->setPropertyName("Plane");
    option->setWidgetType(FilterParameter::ChoiceWidget);
    option->setValueType("unsigned int");
    QVector<QString> choices;
    choices.push_back("XY");
    choices.push_back("XZ");
    choices.push_back("YZ");
    option->setChoices(choices);
    parameters.push_back(option);
  }
  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
void FindLargestCrossSections::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE BEGIN*/
  setPlane( reader->readValue("Plane", getPlane()));
  /* FILTER_WIDGETCODEGEN_AUTO_GENERATED_CODE END*/
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int FindLargestCrossSections::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  writer->writeValue("Plane", getPlane());
  writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLargestCrossSections::dataCheck(bool preflight, size_t voxels, size_t features, size_t ensembles)
{
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  QVector<int> dims(1, 1);
  m_FeatureIdsPtr = m->getPrereqArray<int32_t, AbstractFilter>(this, m_CellAttributeMatrixName,  m_FeatureIdsArrayName, -300, voxels, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_FeatureIdsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_FeatureIds = m_FeatureIdsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
  m_LargestCrossSectionsPtr = m->createNonPrereqArray<float, AbstractFilter>(this, m_CellFeatureAttributeMatrixName,  m_LargestCrossSectionsArrayName, 0, features, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_LargestCrossSectionsPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
{ m_LargestCrossSections = m_LargestCrossSectionsPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLargestCrossSections::preflight()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    addErrorMessage(getHumanLabel(), "The VolumeDataContainer Object with the specific name " + getDataContainerName() + " was not available.", getErrorCondition());
    return;
  }

  dataCheck(true, 1, 1, 1);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLargestCrossSections::execute()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());
  if(NULL == m)
  {
    setErrorCondition(-999);
    notifyErrorMessage("The DataContainer Object was NULL", -999);
    return;
  }
  setErrorCondition(0);

  dataCheck(false, totalPoints, totalFeatures, totalEnsembles);
  if (getErrorCondition() < 0)
  {
    return;
  }

  if(m->getXPoints() > 1 && m->getYPoints() > 1 && m->getZPoints() > 1) { find_crosssections(); }
  else
  {
    setErrorCondition(-999);
    notifyErrorMessage("The volume is not 3D and cannot be run through this filter", -999);
    return;
  }
  notifyStatusMessage("FindLargestCrossSections Completed");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FindLargestCrossSections::find_crosssections()
{
  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getDataContainerName());

  size_t numfeatures = m->getAttributeMatrix(getCellFeatureAttributeMatrixName())->getNumTuples();

  DataArray<double>::Pointer m_FeatureCounts = DataArray<double>::CreateArray(numfeatures, "FeatureCounts");
  double* featurecounts = m_FeatureCounts->getPointer(0);

  int outPlane, inPlane1, inPlane2;
  float res_scalar, area;
  int stride1, stride2, stride3;
  int istride, jstride, kstride;
  int point, gnum;

  if(m_Plane == 0)
  {
    outPlane = m->getZPoints();
    inPlane1 = m->getXPoints();
    inPlane2 = m->getYPoints();
    res_scalar = m->getXRes() * m->getYRes();
    stride1 = inPlane1 * inPlane2;
    stride2 = 1;
    stride3 = inPlane1;
  }
  if(m_Plane == 1)
  {
    outPlane = m->getYPoints();
    inPlane1 = m->getXPoints();
    inPlane2 = m->getZPoints();
    res_scalar = m->getXRes() * m->getZRes();
    stride1 = inPlane1;
    stride2 = 1;
    stride3 = inPlane1 * inPlane2;
  }
  if(m_Plane == 2)
  {
    outPlane = m->getXPoints();
    inPlane1 = m->getYPoints();
    inPlane2 = m->getZPoints();
    res_scalar = m->getYRes() * m->getZRes();
    stride1 = 1;
    stride2 = inPlane1;
    stride3 = inPlane1 * inPlane2;
  }
  for(int i = 0; i < outPlane; i++)
  {
    for (size_t g = 0; g < numfeatures * 1; g++)
    {
      featurecounts[g] = 0.0f;
    }
    istride = i * stride1;
    for (int j = 0; j < inPlane1; j++)
    {
      jstride = j * stride2;
      for(int k = 0; k < inPlane2; k++)
      {
        kstride = k * stride3;
        point = istride + jstride + kstride;
        gnum = m_FeatureIds[point];
        featurecounts[gnum]++;
      }
    }
    for (size_t g = 1; g < numfeatures; g++)
    {
      area = featurecounts[g] * res_scalar;
      if(area > m_LargestCrossSections[g]) { m_LargestCrossSections[g] = area; }
    }
  }
}


