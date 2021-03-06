
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkBinaryBallStructuringElement.h"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkDilateObjectMorphologyImageFilter.h"
#include "itkParabolicDilateImageFilter.h"


/**
 * ******************* dilationGrayscale *******************
 */

template< class ImageType >
void dilationGrayscale(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned int> & radius,
  const std::string & boundaryCondition,
  const bool useCompression )
{
  /** Typedefs. */
  typedef typename ImageType::PixelType               PixelType;
  const unsigned int Dimension = ImageType::ImageDimension;
  typedef itk::ImageFileReader< ImageType >           ReaderType;
  typedef itk::ImageFileWriter< ImageType >           WriterType;
  typedef itk::BinaryBallStructuringElement<
    PixelType, Dimension >                            StructuringElementType;
  typedef typename StructuringElementType::RadiusType RadiusType;
  typedef itk::GrayscaleDilateImageFilter<
    ImageType, ImageType, StructuringElementType >    DilateFilterType;
  typedef typename
    DilateFilterType::DefaultBoundaryConditionType    BoundaryConditionType;

  /** Declarations. */
  typename ReaderType::Pointer reader = ReaderType::New();
  typename WriterType::Pointer writer = WriterType::New();
  typename DilateFilterType::Pointer dilation = DilateFilterType::New();

  /** Setup the reader. */
  reader->SetFileName( inputFileName.c_str() );

  /** Set a boundary condition value.
   * This is the value outside the image.
   * By default it is set to min(PixelType).
   */

  PixelType boundaryValue = itk::NumericTraits<PixelType>::NonpositiveMin();
  if( boundaryCondition != "")
  {
    if( itk::NumericTraits<PixelType>::is_integer )
    {
      boundaryValue = static_cast<PixelType>( atoi( boundaryCondition.c_str() ) );
    }
    else
    {
      boundaryValue = static_cast<PixelType>( atof( boundaryCondition.c_str() ) );
    }

    dilation->SetBoundary(boundaryValue);
  }

  /** Create the structuring element. */
  RadiusType  radiusarray;
  for( unsigned int i = 0; i < Dimension; i++ )
  {
    radiusarray.SetElement( i, radius[ i ] );
  }
  StructuringElementType  S_ball;
  S_ball.SetRadius( radiusarray );
  S_ball.CreateStructuringElement();

  /** Setup the dilation filter. */
  dilation->SetKernel( S_ball );
  dilation->SetInput( reader->GetOutput() );

  /** Write the output image. */
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( dilation->GetOutput() );
  writer->SetUseCompression( useCompression );
  writer->Update();

} // end dilationGrayscale()


  /**
   * ******************* dilationBinary *******************
   */

template< class ImageType >
void dilationBinary(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned int> & radius,
  const std::vector<std::string> & bin,
  const bool useCompression )
{
  /** Typedefs. */
  typedef typename ImageType::PixelType               PixelType;
  const unsigned int Dimension = ImageType::ImageDimension;
  typedef itk::ImageFileReader< ImageType >           ReaderType;
  typedef itk::ImageFileWriter< ImageType >           WriterType;
  typedef itk::BinaryBallStructuringElement<
    PixelType, Dimension >                            StructuringElementType;
  typedef typename StructuringElementType::RadiusType RadiusType;
  typedef itk::BinaryDilateImageFilter<
    ImageType, ImageType, StructuringElementType >    DilateFilterType;

  /** Declarations. */
  typename ReaderType::Pointer reader = ReaderType::New();
  typename WriterType::Pointer writer = WriterType::New();
  typename DilateFilterType::Pointer dilation = DilateFilterType::New();

  /** Setup the reader. */
  reader->SetFileName( inputFileName.c_str() );

  /** Get foreground, background and erosion values. */
  std::vector<PixelType> values( 3 );
  values[ 0 ] = itk::NumericTraits<PixelType>::One;
  values[ 1 ] = itk::NumericTraits<PixelType>::Zero;
  if( bin.size() == 2 )
  {
    for( unsigned int i = 0; i < 2; ++i )
    {
      if( itk::NumericTraits<PixelType>::is_integer )
      {
        values[ i ] = static_cast<PixelType>( atoi( bin[ i ].c_str() ) );
      }
      else
      {
        values[ i ] = static_cast<PixelType>( atof( bin[ i ].c_str() ) );
      }
    }
  }

  /** Create the structuring element. */
  RadiusType  radiusarray;
  for( unsigned int i = 0; i < Dimension; i++ )
  {
    radiusarray.SetElement( i, radius[ i ] );
  }
  StructuringElementType  S_ball;
  S_ball.SetRadius( radiusarray );
  S_ball.CreateStructuringElement();

  /** Setup the dilation filter. */
  dilation->SetForegroundValue( values[ 0 ] );
  dilation->SetBackgroundValue( values[ 1 ] );
  //dilation->SetDilateValue( values[ 2 ] );
  dilation->SetBoundaryToForeground( false );
  dilation->SetKernel( S_ball );
  dilation->SetInput( reader->GetOutput() );

  /** Write the output image. */
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( dilation->GetOutput() );
  writer->SetUseCompression( useCompression );
  writer->Update();

} // end dilationBinary()


  /**
   * ******************* dilationBinaryObject *******************
   * Don't use for now, because it does not give output, consistent
   * with the grayscale and binary.
   */

template< class ImageType >
void dilationBinaryObject(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned int> & radius,
  const std::string & boundaryCondition )
{
  /** Typedefs. */
  typedef typename ImageType::PixelType               PixelType;
  const unsigned int Dimension = ImageType::ImageDimension;
  typedef itk::ImageFileReader< ImageType >           ReaderType;
  typedef itk::ImageFileWriter< ImageType >           WriterType;
  typedef itk::BinaryBallStructuringElement<
    PixelType, Dimension >                            StructuringElementType;
  typedef typename StructuringElementType::RadiusType RadiusType;
  typedef itk::DilateObjectMorphologyImageFilter<
    ImageType, ImageType, StructuringElementType >    FilterType;
  typedef typename
    FilterType::DefaultBoundaryConditionType    BoundaryConditionType;

  /** Declarations. */
  typename ReaderType::Pointer reader = ReaderType::New();
  typename WriterType::Pointer writer = WriterType::New();
  typename FilterType::Pointer filter = FilterType::New();

  /** Setup the reader. */
  reader->SetFileName( inputFileName.c_str() );

  /** Create and fill the radius. */
  RadiusType  radiusarray;
  radiusarray.Fill( 1 );
  for( unsigned int i = 0; i < Dimension; i++ )
  {
    radiusarray.SetElement( i, radius[ i ] );
  }

  /** Create the structuring element and set it into the filter filter. */
  StructuringElementType  S_ball;
  S_ball.SetRadius( radiusarray );
  S_ball.CreateStructuringElement();
  filter->SetKernel( S_ball );

  /** Set a boundary condition value. This is the value outside the image.
   * By default it is set to min(PixelType). */
  BoundaryConditionType bc;
  PixelType bcValue = itk::NumericTraits<PixelType>::NonpositiveMin();
  if( boundaryCondition != "")
  {
    if( itk::NumericTraits<PixelType>::is_integer )
    {
      bcValue = static_cast<PixelType>( atoi( boundaryCondition.c_str() ) );
    }
    else
    {
      bcValue = static_cast<PixelType>( atof( boundaryCondition.c_str() ) );
    }
    bc.SetConstant( bcValue );
    filter->OverrideBoundaryCondition(&bc);
  }

  /** Connect the pipeline. */
  filter->SetInput( reader->GetOutput() );

  /** Write the output image. */
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( filter->GetOutput() );
  writer->Update();

} // end dilationBinaryObject



/**
 * ******************* dilationParabolic *******************
 */

template< class ImageType >
void dilationParabolic(
  const std::string & inputFileName,
  const std::string & outputFileName,
  const std::vector<unsigned int> & radius,
  const bool useCompression )
{
  /** Typedefs. */
  typedef typename ImageType::PixelType               PixelType;
  const unsigned int Dimension = ImageType::ImageDimension;
  typedef itk::ImageFileReader< ImageType >           ReaderType;
  typedef itk::ImageFileWriter< ImageType >           WriterType;
  typedef itk::ParabolicDilateImageFilter<
    ImageType, ImageType >                            FilterType;
  typedef typename FilterType::RadiusType             RadiusType;
  typedef typename FilterType::ScalarRealType         ScalarRealType;

  /** Declarations. */
  typename ReaderType::Pointer reader = ReaderType::New();
  typename WriterType::Pointer writer = WriterType::New();
  typename FilterType::Pointer filter = FilterType::New();

  /** Setup the reader. */
  reader->SetFileName( inputFileName.c_str() );

  /** Get the correct radius. */
  RadiusType      radiusArray;
  ScalarRealType  radius1D = 0.0;
  for( unsigned int i = 0; i < Dimension; ++i )
  {
    // Very specific computation for the parabolic filter:
    radius1D = static_cast<ScalarRealType>( radius[ i ] ) ;//+ 1.0;
    radius1D = radius1D * radius1D / 2.0 + 1.0;
    radiusArray.SetElement( i, radius1D );
  }

  /** Setup the filter. */
  filter->SetUseImageSpacing( false );
  filter->SetScale( radiusArray );
  filter->SetInput( reader->GetOutput() );

  /** Write the output image. */
  writer->SetFileName( outputFileName.c_str() );
  writer->SetInput( filter->GetOutput() );
  writer->SetUseCompression( useCompression );
  writer->Update();

} // end dilationParabolic()

