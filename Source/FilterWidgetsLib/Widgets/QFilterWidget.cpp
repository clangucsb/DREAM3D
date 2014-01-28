/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "QFilterWidget.h"



#include <QtCore/QTimer>
#include <QtCore/QResource>
#include <QtCore/QDir>

#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>


#include "QtSupport/DREAM3DHelpUrlGenerator.h"

#include "DREAM3DLib/Common/FilterManager.h"
#include "DREAM3DLib/Common/IFilterFactory.hpp"
#include "DREAM3DLib/Common/FilterFactory.hpp"



#include "FilterWidgetsLib/FilterWidgetManager.h"


// -----------------------------------------------------------------------------
// Include the "moc" file that was generated for this file
// -----------------------------------------------------------------------------
#include "FilterWidgetsLib/Widgets/moc_QFilterWidget.cxx"

#define PADDING 5
#define BORDER 2
#define IMAGE_WIDTH 17
#define IMAGE_HEIGHT 17

// These Streams need to be implemented so that our 3 Element Vectors can be read/write to disk/prefs files
QDataStream& operator<<( QDataStream& out, const IntVec3_t& v)
{
  out << v.x << v.y << v.z;
  return out;
}
QDataStream& operator>>( QDataStream& in, IntVec3_t& v) { in >> v.x >> v.y >> v.z; return in; }

QDataStream& operator<<( QDataStream& out, const FloatVec3_t& v) { out << v.x << v.y << v.z; return out; }
QDataStream& operator>>( QDataStream& in, FloatVec3_t& v) { in >> v.x >> v.y >> v.z; return in; }

// Initialize private static member variable
QString QFilterWidget::m_OpenDialogLastDirectory = "";



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget::QFilterWidget(QWidget* parent) :
  QFrame(parent),
  m_CurrentBorderColorFactor(0),
  m_BorderIncrement(16),
  m_IsSelected(false),
  m_HasPreflightErrors(false),
  m_HasPreflightWarnings(false),
  m_BasicInputsScrollWidget(NULL),
  m_Observer(NULL)
{
  initialize(AbstractFilter::NullPointer() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget::QFilterWidget(AbstractFilter::Pointer filter, IObserver* observer, QWidget* parent) :
  QFrame(parent),
  m_CurrentBorderColorFactor(0),
  m_BorderIncrement(16),
  m_IsSelected(false),
  m_HasPreflightErrors(false),
  m_HasPreflightWarnings(false),
  m_BasicInputsScrollWidget(NULL),
  m_Observer(observer)
{
  initialize(filter);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::initialize(AbstractFilter::Pointer filter)
{
  setupUi(this);

  qRegisterMetaType<IntVec3_t>("IntVec3_t");
  qRegisterMetaTypeStreamOperators<IntVec3_t>("IntVec3_t");

  qRegisterMetaType<FloatVec3_t>("FloatVec3_t");
  qRegisterMetaTypeStreamOperators<FloatVec3_t>("FloatVec3_t");

  if ( m_OpenDialogLastDirectory.isEmpty() )
  {
    m_OpenDialogLastDirectory = QDir::homePath();
  }

  m_DeleteRect.setX(PADDING + BORDER);
  m_DeleteRect.setY(PADDING + BORDER);
  m_DeleteRect.setWidth(IMAGE_WIDTH);
  m_DeleteRect.setHeight(IMAGE_HEIGHT);
  m_timer = new QTimer(this);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(changeStyle()));

  // Set the AbstractFilter for this class
  m_Filter = filter;


  // If the filter is valid then instantiate all the FilterParameterWidgets
  if (NULL != m_Filter.get())
  {
    // Create the Widget that will be placed into the Basic Inputs Scroll Area
    m_BasicInputsScrollWidget = new QWidget();
    QString name = QString::fromUtf8("basicInputsScrollWidget_") + m_Filter->getNameOfClass();
    m_BasicInputsScrollWidget->setObjectName(name);
    m_BasicInputsScrollWidget->setGeometry(QRect(0, 0, 250, 267));
    QVBoxLayout* verticalLayout = new QVBoxLayout(m_BasicInputsScrollWidget);
    name = QString::fromUtf8("verticalLayout") + m_Filter->getNameOfClass();
    verticalLayout->setObjectName(name);


    // Set the Name of the filter into the FilterWidget
    filterName->setText(m_Filter->getHumanLabel() );

    // Create all the FilterParameterWidget objects that can be displayed where ever the developer needs
    FilterWidgetManager::Pointer fwm = FilterWidgetManager::Instance();

    QVector<FilterParameter::Pointer> options = m_Filter->getFilterParameters();
    for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
    {

      FilterParameter* option = (*iter).get();

      QWidget* w = fwm->createWidget(option, m_Filter.get());

      if (NULL == w) continue;
      m_FilterParameterWidgets.push_back(w);
      // Set the parent for the widget
      w->setParent(m_BasicInputsScrollWidget);
      // Add the FilterWidget to the layout
      verticalLayout->addWidget(w);

      // Connect any errors/warnings that the widget may create to the Issues Tab
    }

    QSpacerItem* verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    verticalLayout->addItem(verticalSpacer);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterWidget::~QFilterWidget()
{
//  std::cout << "~QFilterWidget() " << m_Filter->getNameOfClass().toStdString() << std::endl;
    delete m_BasicInputsScrollWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::on_helpBtn_clicked()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QFilterWidget::getHumanLabel()
{
  if (NULL != m_Filter.get())
    return m_Filter->getHumanLabel();

  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QFilterWidget::getFilterGroup()
{
  if (NULL != m_Filter.get())
    return m_Filter->getGroupName();

  return "";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QFilterWidget::getFilterSubGroup()
{
  if (NULL != m_Filter.get())
    return m_Filter->getSubGroupName();

  return "";
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QVector<QWidget*>& QFilterWidget::getFilterParameterWidgets()
{
  return m_FilterParameterWidgets;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* QFilterWidget::getScrollWidgetContents()
{
  return m_BasicInputsScrollWidget;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setBorderColorStyle(QString s)
{
  m_BorderColorStyle = s;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString QFilterWidget::getBorderColorStyle()
{
  return m_BorderColorStyle;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setHasPreflightErrors(bool hasErrors)
{
  m_HasPreflightErrors = hasErrors;
  if (m_HasPreflightErrors == true)
  {
    //    m_timer->start(100);
    //    m_CurrentBorderColorFactor = 64;
    //    m_BorderIncrement = 16;
  }
  else
  {
    m_timer->stop();
  }
  changeStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setHasPreflightWarnings(bool hasWarnings)
{
  m_HasPreflightWarnings = hasWarnings;
  if (m_HasPreflightWarnings == true)
  {
    //    m_timer->start(100);
    //    m_CurrentBorderColorFactor = 64;
    //    m_BorderIncrement = 16;
  }
  else
  {
    m_timer->stop();
  }
  changeStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::setIsSelected(bool b)
{
  m_IsSelected = b;
  changeStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QFilterWidget::isSelected()
{
  return m_IsSelected;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::changeStyle()
{
  QString style;
  if (m_HasPreflightErrors == true)
  {
    m_CurrentBorderColorFactor += m_BorderIncrement;
    if (m_CurrentBorderColorFactor > 127)
    {
      m_BorderIncrement = -16;
    }
    if (m_CurrentBorderColorFactor < 1)
    {
      m_BorderIncrement = 16;
    }

    style.append("border: 2px solid rgb(");
    style.append(QString::number(255 - m_CurrentBorderColorFactor, 10));
    style.append(", ");
    style.append(QString::number(m_CurrentBorderColorFactor, 10));
    style.append(", ");
    style.append(QString::number(m_CurrentBorderColorFactor, 10));
    style.append(");");
  }
  else if(m_HasPreflightWarnings)
  {
    style.append("border: 2px solid rgb(172, 168, 0);");
  }
  else if(m_IsSelected == true )
  {
    style.append("border: 2px solid purple;");
    m_CurrentBorderColorFactor = 0;
  }
  else
  {
    style.append("border: 1px solid #515151;");
    style.append("margin: 1px;");
    m_CurrentBorderColorFactor = 0;
  }
  setBorderColorStyle(style);
  updateWidgetStyle();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::updateWidgetStyle()
{
  QString style;

  style.append("QFrame#QFilterWidget\n {\n");

  style.append("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(185, 185, 185, 255), stop:0.5 rgba(226, 226, 226, 255), stop:1 rgba(150, 150, 150, 255));\n");

#if 0
  QString headerFile(":/QFilterWidgetHeaders/");

  headerFile.append(getFilterGroup());
  headerFile.append("_Header.png");
  QResource headerResource(headerFile);
  if (true == headerResource.isValid())
  {
    style.append("background-image: url(");
    style.append( headerFile );
    style.append(");\n");
  }
  else
#endif


    //    QString headerImage("background-image: url(:/filterWidgetBorder.png);");
    //  if(m_HasPreflightWarnings == true)
    //  {
    //    headerImage = "background-image: url(:/filterWidgetBorder_Warning.png);";
    //  }

    //  if(m_HasPreflightErrors == true)
    //  {
    //    headerImage = "background-image: url(:/filterWidgetBorder_Error.png);";
    //  }
    //  style.append(headerImage);

    style.append("background-position: top ;\n background-repeat: repeat-x;");

  style.append(getBorderColorStyle());

  style.append("border-radius: 10px;");
  style.append("padding: 0 0 0 0px;");
#if defined(Q_WS_WIN)
  style.append("font: 85 italic 10pt \"Arial\";");
#elif defined(Q_WS_MAC)
  style.append("font: 100 italic 12pt \"Arial\";");
#else
  style.append("font: 85 italic 9pt \"Arial\";");
#endif
  style.append("font-weight: bold;");
  style.append("}\n");
  //  style.append(" QGroupBox::title {");
  //  style.append("    subcontrol-origin: padding;");
  //  style.append("    subcontrol-position: top left;");
  //  style.append("    padding: 5 5px;");
  //  style.append("    background-color: rgba(255, 255, 255, 0);");
  //  style.append(" }\n");
  //  style.append("QGroupBox::indicator {");
  //  style.append("    width: 17px;");
  //  style.append("    height: 17px;");
  //  style.append("}\n");
  //  style.append("\nQGroupBox::indicator:unchecked { image: url(:/delete-corner.png);}");
  //  style.append("\nQGroupBox::indicator:unchecked:pressed { image: url(:/delete-corner-pressed.png);}");
  //  style.append("\nQGroupBox::indicator:checked { image: url(:/delete-corner.png);}");
  //  style.append("\nQGroupBox::indicator:checked:pressed { image: url(:/delete-corner-pressed.png);}");

  setStyleSheet(style);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer QFilterWidget::getFilter()
{
  return m_Filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void  QFilterWidget::mousePressEvent ( QMouseEvent* event )
{
  if(event->button() != Qt::LeftButton)
  {
    event->ignore();
    return;
  }
  // Only if we are inside the delete checkbox/image then pass up to the superclass
  if(m_DeleteRect.contains(event->pos()))
  {
    QFrame::mousePressEvent(event);
  }
  else
  {
    dragStartPosition = event->pos();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::mouseReleaseEvent(QMouseEvent* event)
{
  if(event->button() != Qt::LeftButton)
  {
    event->ignore();
    return;
  }
  // Only if we are inside the delete checkbox/image then pass up to the superclass
  if(m_DeleteRect.contains(event->pos()))
  {
    QFrame::mouseReleaseEvent(event);
  }
  else
  {
    emit widgetSelected(this);
    event->setAccepted(true);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::mouseMoveEvent(QMouseEvent* event)
{
  if(!(event->buttons() & Qt::LeftButton))
  {
    return;
  }
  if((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
  {
    return;
  }

  QPixmap pixmap = QPixmap::grabWidget(this);

  // Create new picture for transparent
  QPixmap transparent(pixmap.size());
  // Do transparency
  transparent.fill(Qt::transparent);
#if 1
  QPainter p;

  p.begin(&transparent);
  p.setOpacity(0.70);
  // p.setCompositionMode(QPainter::CompositionMode_Plus);
  p.drawPixmap(0, 0, pixmap);
  // p.setCompositionMode(QPainter::CompositionMode_Plus);

  // Set transparency level to 150 (possible values are 0-255)
  // The alpha channel of a color specifies the transparency effect,
  // 0 represents a fully transparent color, while 255 represents
  // a fully opaque color.
  //  p.fillRect(transparent.rect(), QColor(0, 0, 0, 150));
  p.end();
#endif


  QByteArray itemData;
  QDataStream dataStream(&itemData, QIODevice::WriteOnly);
  dataStream << transparent << QPoint(event->pos());

  QMimeData* mimeData = new QMimeData;
  mimeData->setData("application/x-dnditemdata", itemData);

  QDrag* drag = new QDrag(this);
  drag->setMimeData(mimeData);
  drag->setPixmap(transparent);
  drag->setHotSpot(event->pos());

  emit dragStarted(this);

  //  if(drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction)
  //  {
  //    qDebug() << "Drag should close the widget because it was MOVE" << "\n";
  //  }
  //  else
  //  {
  //    qDebug() << "Drag should leave Widget alone because it was COPY" << "\n";
  //  }
  drag->exec(Qt::MoveAction);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::getGuiParametersFromFilter(AbstractFilter* filt)
{
  BOOST_ASSERT("QFilterWidget::getGuiParametersFromFilter executed");    // Code should never enter this function
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterWidget::on_deleteBtn_clicked()
{
  emit filterWidgetRemoved(this);
}