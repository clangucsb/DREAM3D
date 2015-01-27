/*
 * Your License or Copyright Information can go here
 */

#include "DetermineStitchingCoordinatesGeneric.h"

#include <QtCore/QString>

#include "ZeissImport/ZeissImportConstants.h"

#include "ImageProcessing/ImageProcessingFilters/util/DetermineStitching.h"

#include "DREAM3DLib/DataArrays/StringDataArray.hpp"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DetermineStitchingCoordinatesGeneric::DetermineStitchingCoordinatesGeneric() :
  AbstractFilter(),
  m_AttributeMatrixName(DREAM3D::Defaults::VolumeDataContainerName, DREAM3D::Defaults::CellFeatureAttributeMatrixName, ""),
  m_UseZeissMetaData(false),
  m_MetaDataAttributeMatrixName("Tile AttributeMatrix"),
  m_TileCalculatedInfoAttributeMatrixName("Tile Info AttrMat"),
  m_StitchedCoordinatesArrayName("Stitched Coordinates"),
  m_StitchedArrayNames("Stitched Array Names")
{
  setupFilterParameters();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DetermineStitchingCoordinatesGeneric::~DetermineStitchingCoordinatesGeneric()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DetermineStitchingCoordinatesGeneric::setupFilterParameters()
{
  FilterParameterVector parameters;
  parameters.push_back(FilterParameter::New("Image Data Attribute Matrix", "AttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getAttributeMatrixName(), false, ""));
  QStringList linkedProps;
  linkedProps << "MetaDataAttributeMatrixName";
  parameters.push_back(LinkedBooleanFilterParameter::New("Use Zeiss Meta Data", "UseZeissMetaData", getUseZeissMetaData(), linkedProps, false));
  parameters.push_back(FilterParameter::New("Zeiss Meta Data Attribute Matrix", "MetaDataAttributeMatrixName", FilterParameterWidgetType::AttributeMatrixSelectionWidget, getMetaDataAttributeMatrixName(), false, ""));

  parameters.push_back(FilterParameter::New("Created Data Parameters", "", FilterParameterWidgetType::SeparatorWidget, "", true));

  parameters.push_back(FilterParameter::New("Stitched Attribute Matrix Name", "TileCalculatedInfoAttributeMatrixName", FilterParameterWidgetType::StringWidget, getTileCalculatedInfoAttributeMatrixName(), true, ""));
  parameters.push_back(FilterParameter::New("Stitched Coordinates Data Array", "StitchedCoordinatesArrayName", FilterParameterWidgetType::StringWidget, getStitchedCoordinatesArrayName(), true, ""));
  parameters.push_back(FilterParameter::New("Stitched Coordinates Data Array Names", "StitchedArrayNames", FilterParameterWidgetType::StringWidget, getStitchedArrayNames(), true, ""));

  setFilterParameters(parameters);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DetermineStitchingCoordinatesGeneric::readFilterParameters(AbstractFilterParametersReader* reader, int index)
{
  reader->openFilterGroup(this, index);
  setAttributeMatrixName(reader->readDataArrayPath("AttributeMatrixName", getAttributeMatrixName()));
  setUseZeissMetaData(reader->readValue("UseZeissMetaData", getUseZeissMetaData()));
  setMetaDataAttributeMatrixName(reader->readDataArrayPath("MetaDataAttributeMatrixName", getMetaDataAttributeMatrixName()));

  setTileCalculatedInfoAttributeMatrixName(reader->readString("TileCalculatedInfoAttributeMatrixName", getTileCalculatedInfoAttributeMatrixName()));
  setStitchedCoordinatesArrayName(reader->readString("StitchedCoordinatesArrayName", getStitchedCoordinatesArrayName()));
  setStitchedArrayNames(reader->readString("DataArrayNamesForStitchedCoordinates", getStitchedArrayNames()));
  reader->closeFilterGroup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DetermineStitchingCoordinatesGeneric::writeFilterParameters(AbstractFilterParametersWriter* writer, int index)
{
  writer->openFilterGroup(this, index);
  DREAM3D_FILTER_WRITE_PARAMETER(AttributeMatrixName)
      DREAM3D_FILTER_WRITE_PARAMETER(UseZeissMetaData)
      DREAM3D_FILTER_WRITE_PARAMETER(MetaDataAttributeMatrixName)
      DREAM3D_FILTER_WRITE_PARAMETER(TileCalculatedInfoAttributeMatrixName)
      DREAM3D_FILTER_WRITE_PARAMETER(StitchedCoordinatesArrayName)
      DREAM3D_FILTER_WRITE_PARAMETER(StitchedArrayNames)
      writer->closeFilterGroup();
  return ++index; // we want to return the next index that was just written to
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DetermineStitchingCoordinatesGeneric::dataCheck()
{
  setErrorCondition(0);
  DataArrayPath tempPath;

  QString ss;

  AttributeMatrix::Pointer am = getDataContainerArray()->getAttributeMatrix(m_AttributeMatrixName);

  if (am.get() == NULL)
  {
    setErrorCondition(-76000);
    notifyErrorMessage(getHumanLabel(), "The attribute matrix has not been selected properly", -76000);
    return;
  }

  QList<QString> names = am->getAttributeArrayNames();


  QVector<size_t> dims(1, 1);



  m_PointerList.resize(names.size());

  for(int i = 0; i < names.size(); i++)
  {
    tempPath.update(getAttributeMatrixName().getDataContainerName(), getAttributeMatrixName().getAttributeMatrixName(), names[i]);
    m_SelectedCellArrayPtr = getDataContainerArray()->getExistingPrereqArrayFromPath<DataArray<ImageProcessing::DefaultPixelType>, AbstractFilter>(this, tempPath);

    if( NULL != m_SelectedCellArrayPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
    { m_SelectedCellArray = m_SelectedCellArrayPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

    m_PointerList[i] = m_SelectedCellArray;

  }

  if(m_UseZeissMetaData == true)
  {
    AttributeMatrix::Pointer MetaDataAm = getDataContainerArray()->getAttributeMatrix(m_MetaDataAttributeMatrixName);
    if(NULL == MetaDataAm.get())
    {
      notifyErrorMessage(getHumanLabel(), "The Attribute Matrix was not found", -76001);
      return;
    }
    //        QString temp = "_META_DATA";
    bool a = getMetaDataAttributeMatrixName().getAttributeMatrixName().contains("_META_DATA");
    if (a == false)
    {
      notifyErrorMessage(getHumanLabel(), "The Attribute Matrix does not contain the Zeiss Meta Data", -76002);
      return;
    }

  }

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getAttributeMatrixName().getDataContainerName());
  if(getErrorCondition() < 0 || NULL == m) { return; }

  QVector<size_t> tDims(1, m_PointerList.size());
  AttributeMatrix::Pointer AttrMat = m->createNonPrereqAttributeMatrix<AbstractFilter>(this, getTileCalculatedInfoAttributeMatrixName(), tDims, DREAM3D::AttributeMatrixType::CellFeature);
  if(getErrorCondition() < 0) { return; }

  dims[0]=2;

  tempPath.update(getAttributeMatrixName().getDataContainerName(), getTileCalculatedInfoAttributeMatrixName(), getStitchedCoordinatesArrayName());
  m_StitchedCoordinatesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<DataArray<float>, AbstractFilter, float>(this,  tempPath, 0, dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_StitchedCoordinatesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  { m_StitchedCoordinates = m_StitchedCoordinatesPtr.lock()->getPointer(0); } /* Now assign the raw pointer to data from the DataArray<T> object */

  dims[0]=1;



  tempPath.update(getAttributeMatrixName().getDataContainerName(), getTileCalculatedInfoAttributeMatrixName(), getStitchedArrayNames() );
  m_DataArrayNamesForStitchedCoordinatesPtr = getDataContainerArray()->createNonPrereqArrayFromPath<StringDataArray, AbstractFilter, std::string>(this,  tempPath, "0", dims); /* Assigns the shared_ptr<> to an instance variable that is a weak_ptr<> */
  if( NULL != m_DataArrayNamesForStitchedCoordinatesPtr.lock().get() ) /* Validate the Weak Pointer wraps a non-NULL pointer to a DataArray<T> object */
  {
    //   m_DataArrayNamesForStitchedCoordinates = m_DataArrayNamesForStitchedCoordinatesPtr.lock()->getPointer(0);  /* Now assign the raw pointer to data from the DataArray<T> object */
  }

  return;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DetermineStitchingCoordinatesGeneric::preflight()
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
void DetermineStitchingCoordinatesGeneric::execute()
{
  int err = 0;
  dataCheck();
  if(getErrorCondition() < 0) { return; }
  setErrorCondition(0);

  VolumeDataContainer* m = getDataContainerArray()->getDataContainerAs<VolumeDataContainer>(getAttributeMatrixName().getDataContainerName());
  QString attrMatName = getAttributeMatrixName().getAttributeMatrixName();

  QString XTileIndexName = "ImageIndexU";
  QString YTileIndexName = "ImageIndexV";
  QString XGlobalIndexName = "StagePositionX";
  QString YGlobalIndexName = "StagePositionY";
  QString XScale = "ScaleFactorForX";
  QString YScale = "ScaleFactorForY";

  QVector<qint32> xTileList(m_PointerList.size());
  QVector<qint32> yTileList(m_PointerList.size());
  QVector<float> xGlobCoordsList(m_PointerList.size());
  QVector<float> yGlobCoordsList(m_PointerList.size());

  AttributeMatrix::Pointer attrMat = m->getAttributeMatrix(attrMatName);

  xTileList = extractIntegerValues(XTileIndexName);
  if(getErrorCondition() < 0) { return; }
  yTileList = extractIntegerValues(YTileIndexName);
  if(getErrorCondition() < 0) { return; }

  QVector<float> scaleFactors = extractFloatValues(XScale);
  xGlobCoordsList = extractFloatValues(XGlobalIndexName);
  for(qint32 i = 0; i < xGlobCoordsList.size(); i++) {
    xGlobCoordsList[i] /= scaleFactors[i];
  }

  scaleFactors = extractFloatValues(YScale);
  yGlobCoordsList = extractFloatValues(YGlobalIndexName);
  for(qint32 i = 0; i < yGlobCoordsList.size(); i++) {
    yGlobCoordsList[i] /= scaleFactors[i];
  }

  float sampleOrigin[3];
  float voxelResolution[3];

  m->getOrigin(sampleOrigin);
  m->getResolution(voxelResolution);
  QVector<size_t> udims = attrMat->getTupleDimensions();
  size_t totalPoints = attrMat->getNumTuples();


  // Use the helper class to do the actual stitching of the images. There are a lot
  // of parameters so make sure we understand all of them
  FloatArrayType::Pointer temp = DetermineStitching::FindGlobalOrigins(totalPoints, udims,
                                                                       sampleOrigin, voxelResolution,
                                                                       m_PointerList,
                                                                       xGlobCoordsList, yGlobCoordsList,
                                                                       xTileList, yTileList,
                                                                       this);
#if 1
  temp->copyIntoArray(m_StitchedCoordinatesPtr.lock());
#else
  float* src = temp->getPointer(0);
  float* dest = m_StitchedCoordinatesPtr.lock()->getPointer(0);
  size_t totalBytes = (m_StitchedCoordinatesPtr.lock()->getNumberOfTuples() * m_StitchedCoordinatesPtr.lock()->getNumberOfComponents() * sizeof(float));
  ::memcpy(dest, src, totalBytes);
#endif
  StringDataArray::Pointer arrayNames = m_DataArrayNamesForStitchedCoordinatesPtr.lock();
  arrayNames->resize(m_PointerList.size());

  //Create another data array with the list of names of the images in the same order as the returned stitched coordinates
  QList<QString> names = attrMat->getAttributeArrayNames();
  for(size_t i = 0; i < names.size(); i++ )
  {
    arrayNames->setValue(i, names[i]);
  }

  /* Let the GUI know we are done with this filter */
  notifyStatusMessage(getHumanLabel(), "Complete");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<float> DetermineStitchingCoordinatesGeneric::extractFloatValues(QString arrayName)
{
  QVector<float> tileList(m_PointerList.size());
  DataArrayPath tempPath;
  bool ok = false; // Used to make sure we can convert the string into an integer
  tempPath.update(getMetaDataAttributeMatrixName().getDataContainerName(), getMetaDataAttributeMatrixName().getAttributeMatrixName(), arrayName);

  StringDataArray::Pointer metaDataArray = getDataContainerArray()->getExistingPrereqArrayFromPath<StringDataArray, AbstractFilter>(this, tempPath);

  for (size_t i=0; i < m_PointerList.size(); i++)
  {
    QString value = metaDataArray->getValue(i);
    tileList[i] = value.toFloat(&ok);
    if(!ok){
      QString ss = QObject::tr("Error trying to convert the string '%1' to a float. This string was part of the Data Array '%2' at index '%3'.").arg(value).arg(arrayName).arg(i);
      setErrorCondition(-34005);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      tileList.clear();
      return tileList;
    }
  }

  return tileList;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<qint32> DetermineStitchingCoordinatesGeneric::extractIntegerValues(QString arrayName)
{
  QVector<qint32> tileList(m_PointerList.size());
  DataArrayPath tempPath;
  bool ok = false; // Used to make sure we can convert the string into an integer
  tempPath.update(getMetaDataAttributeMatrixName().getDataContainerName(), getMetaDataAttributeMatrixName().getAttributeMatrixName(), arrayName);

  StringDataArray::Pointer metaDataArray = getDataContainerArray()->getExistingPrereqArrayFromPath<StringDataArray, AbstractFilter>(this, tempPath);

  for (size_t i=0; i < m_PointerList.size(); i++)
  {
    QString value = metaDataArray->getValue(i);
    tileList[i] = value.toInt(&ok);
    if(!ok){
      QString ss = QObject::tr("Error trying to convert the string '%1' to an integer. This string was part of the Data Array '%2' at index '%3'.").arg(value).arg(arrayName).arg(i);
      setErrorCondition(-34005);
      notifyErrorMessage(getHumanLabel(), ss, getErrorCondition());
      tileList.clear();
      return tileList;
    }
  }

  return tileList;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DetermineStitchingCoordinatesGeneric::getCompiledLibraryName()
{
  return ZeissImport::ZeissImportBaseName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DetermineStitchingCoordinatesGeneric::getGroupName()
{
  return "ImageProcessing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DetermineStitchingCoordinatesGeneric::getHumanLabel()
{
  return "Determine Stitching Coordinates (Generic)";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
const QString DetermineStitchingCoordinatesGeneric::getSubGroupName()
{
  return "Misc";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer DetermineStitchingCoordinatesGeneric::newFilterInstance(bool copyFilterParameters)
{
  /*
  * write code to optionally copy the filter parameters from the current filter into the new instance
  */
  DetermineStitchingCoordinatesGeneric::Pointer filter = DetermineStitchingCoordinatesGeneric::New();
  if(true == copyFilterParameters)
  {
    /* If the filter uses all the standard Filter Parameter Widgets you can probabaly get
     * away with using this method to copy the filter parameters from the current instance
     * into the new instance
     */
    copyFilterParameterInstanceVariables(filter.get());
    /* If your filter is using a lot of custom FilterParameterWidgets @see ReadH5Ebsd then you
     * may need to copy each filter parameter explicitly plus any other instance variables that
     * are needed into the new instance. Here is some example code from ReadH5Ebsd
     */
    //    DREAM3D_COPY_INSTANCEVAR(OutputFile)
    //    DREAM3D_COPY_INSTANCEVAR(ZStartIndex)
    //    DREAM3D_COPY_INSTANCEVAR(ZEndIndex)
    //    DREAM3D_COPY_INSTANCEVAR(ZResolution)
  }
  return filter;
}

