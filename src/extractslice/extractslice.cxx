/*=========================================================================
*
* Copyright Marius Staring, Stefan Klein, David Doria. 2011.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0.txt
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*=========================================================================*/
/** \file
 \brief Extracts a 2D slice from a 3D image.
 
 \verbinclude extractslice.help
 */
#include "itkCommandLineArgumentParser.h"
#include "ITKToolsHelpers.h"
#include "ITKToolsBase.h"

#include "itkImageFileReader.h"
#include "itkExtractImageFilter.h"
#include "itkImageFileWriter.h"

#include <string>
#include <vector>

#include <itksys/SystemTools.hxx>


/**
 * ******************* GetHelpString *******************
 */

std::string GetHelpString( void )
{
  std::stringstream ss;
  ss << "pxextractslice extracts a 2D slice from a 3D image.\n"
    << "Usage:\n"
    << "pxextractslice\n"
    << "  -in      input image filename\n"
    << "  [-out]   output image filename\n"
    << "  [-pt]    pixel type of input and output images;\n"
    << "           default: automatically determined from the first input image.\n"
    << "  -sn      slice number\n"
    << "  [-d]     the dimension from which a slice is extracted, default the z dimension\n"
    << "Supported pixel types: (unsigned) char, (unsigned) short, float.";

  return ss.str();

} // end GetHelpString()


/** ExtractSlice */

class ITKToolsExtractSliceBase : public itktools::ITKToolsBase
{ 
public:
  ITKToolsExtractSliceBase()
  {
    this->m_InputFileName = "";
    this->m_OutputFileName = "";
    this->m_Slicenumber = 0;
    this->m_WhichDimension = 0;
  };
  ~ITKToolsExtractSliceBase(){};

  /** Input parameters */
  std::string m_InputFileName;
  std::string m_OutputFileName;
  unsigned int m_Slicenumber;
  unsigned int m_WhichDimension;
    
}; // end ExtractSliceBase


template< class TComponentType >
class ITKToolsExtractSlice : public ITKToolsExtractSliceBase
{
public:
  typedef ITKToolsExtractSlice Self;

  ITKToolsExtractSlice(){};
  ~ITKToolsExtractSlice(){};

  static Self * New( itktools::ComponentType componentType )
  {
    if ( itktools::IsType<TComponentType>( componentType ) )
    {
      return new Self;
    }
    return 0;
  }

  void Run( void )
  {
    /** Some typedef's. */
    typedef itk::Image<TComponentType, 3>         Image3DType;
    typedef itk::Image<TComponentType, 2>         Image2DType;
    typedef itk::ImageFileReader<Image3DType>     ImageReaderType;
    typedef itk::ExtractImageFilter<
      Image3DType, Image2DType >                  ExtractFilterType;
    typedef itk::ImageFileWriter<Image2DType>     ImageWriterType;

    typedef typename Image3DType::RegionType      RegionType;
    typedef typename Image3DType::SizeType        SizeType;
    typedef typename Image3DType::IndexType       IndexType;

    /** Create reader. */
    typename ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName( this->m_InputFileName.c_str() );
    reader->Update();

    /** Create extractor. */
    typename ExtractFilterType::Pointer extractor = ExtractFilterType::New();
    extractor->SetInput( reader->GetOutput() );

    /** Get the size and set which_dimension to zero. */
    RegionType inputRegion = reader->GetOutput()->GetLargestPossibleRegion();
    SizeType size = inputRegion.GetSize();
    size[ this->m_WhichDimension ] = 0;

    /** Get the index and set which_dimension to the correct slice. */
    IndexType start = inputRegion.GetIndex();
    start[ this->m_WhichDimension ] = this->m_Slicenumber;

    /** Create a desired extraction region and set it into the extractor. */
    RegionType desiredRegion;
    desiredRegion.SetSize(  size  );
    desiredRegion.SetIndex( start );
    extractor->SetExtractionRegion( desiredRegion );

    /** The direction cosines of the 2D extracted data is set to
     * a submatrix of the 3D input image. */
    extractor->SetDirectionCollapseToSubmatrix();

    /** Write the 2D output image. */
    typename ImageWriterType::Pointer writer = ImageWriterType::New();
    writer->SetFileName( this->m_OutputFileName.c_str() );
    writer->SetInput( extractor->GetOutput() );
    writer->Update();
  } // end Run()

}; // end ExtractSlice

//-------------------------------------------------------------------------------------

int main( int argc, char ** argv )
{
  /** Create a command line argument parser. */
  itk::CommandLineArgumentParser::Pointer parser = itk::CommandLineArgumentParser::New();
  parser->SetCommandLineArguments( argc, argv );
  parser->SetProgramHelpText( GetHelpString() );

  parser->MarkArgumentAsRequired( "-in", "The input filename." );
  parser->MarkArgumentAsRequired( "-sn", "The slice number." );

  itk::CommandLineArgumentParser::ReturnValue validateArguments = parser->CheckForRequiredArguments();

  if( validateArguments == itk::CommandLineArgumentParser::FAILED )
  {
    return EXIT_FAILURE;
  }
  else if( validateArguments == itk::CommandLineArgumentParser::HELPREQUESTED )
  {
    return EXIT_SUCCESS;
  }
  
  /** Get the input file name. */
  std::string inputFileName;
  parser->GetCommandLineArgument( "-in", inputFileName );

  /** Determine input image properties. */
  std::string ComponentType = "short";
  std::string PixelType; //we don't use this
  unsigned int Dimension = 3;
  unsigned int NumberOfComponents = 1;
  std::vector<unsigned int> imagesize( Dimension, 0 );
  int retgip = itktools::GetImageProperties(
    inputFileName,
    PixelType,
    ComponentType,
    Dimension,
    NumberOfComponents,
    imagesize );

  if ( retgip != 0 )
  {
    return 1;
  }

  /** Let the user overrule this. */
  parser->GetCommandLineArgument( "-pt", ComponentType );

  /** Error checking. */
  if ( NumberOfComponents > 1 )
  {
    std::cerr << "ERROR: The NumberOfComponents is larger than 1!" << std::endl;
    std::cerr << "Vector images are not supported!" << std::endl;
    return 1;
  }

  /** Get the slicenumber which is to be extracted. */
  unsigned int slicenumber = 0;
  parser->GetCommandLineArgument( "-sn", slicenumber );

  std::string slicenumberstring;
  parser->GetCommandLineArgument( "-sn", slicenumberstring );

  /** Get the dimension in which the slice is to be extracted.
   * The default is the z-direction.
   */
  unsigned int which_dimension = 2;
  parser->GetCommandLineArgument( "-d", which_dimension );

  /** Sanity check. */
  if ( slicenumber > imagesize[ which_dimension ] )
  {
    std::cerr << "ERROR: You selected slice number "
      << slicenumber
      << ", where the input image only has "
      << imagesize[ which_dimension ]
      << " slices in dimension "
      << which_dimension << "." << std::endl;
    return 1;
  }

  /** Sanity check. */
  if ( which_dimension > Dimension - 1 )
  {
    std::cerr << "ERROR: You selected to extract a slice from dimension "
      << which_dimension + 1
      << ", where the input image is "
      << Dimension
      << "D." << std::endl;
    return 1;
  }

  /** Get the outputFileName. */
  std::string direction = "z";
  if ( which_dimension == 0 ) direction = "x";
  else if ( which_dimension == 1 ) direction = "y";
  std::string part1 =
      itksys::SystemTools::GetFilenameWithoutLastExtension( inputFileName );
  std::string part2 =
    itksys::SystemTools::GetFilenameLastExtension( inputFileName );
  std::string outputFileName = part1 + "_slice_" + direction + "=" + slicenumberstring + part2;
  parser->GetCommandLineArgument( "-out", outputFileName );
  
  /** Class that does the work */
  ITKToolsExtractSliceBase * extractSlice = 0; 

  itktools::ComponentType componentType
    = itk::ImageIOBase::GetComponentTypeFromString( ComponentType );
    
  try
  {    
    // now call all possible template combinations.
    if (!extractSlice) extractSlice = ITKToolsExtractSlice< unsigned char >::New( componentType );
    if (!extractSlice) extractSlice = ITKToolsExtractSlice< char >::New( componentType );
    if (!extractSlice) extractSlice = ITKToolsExtractSlice< unsigned short >::New( componentType );
    if (!extractSlice) extractSlice = ITKToolsExtractSlice< short >::New( componentType );
    if (!extractSlice) extractSlice = ITKToolsExtractSlice< float >::New( componentType );

    if (!extractSlice) 
    {
      std::cerr << "ERROR: this combination of pixeltype and dimension is not supported!" << std::endl;
      std::cerr
        << "pixel (component) type = " << ComponentType
        << " ; dimension = " << Dimension
        << std::endl;
      return 1;
    }

    extractSlice->m_InputFileName = inputFileName;
    extractSlice->m_OutputFileName = outputFileName;
    extractSlice->m_WhichDimension = which_dimension;
    extractSlice->m_Slicenumber = slicenumber;
  
    extractSlice->Run();
    
    delete extractSlice;
  }
  catch( itk::ExceptionObject &e )
  {
    std::cerr << "Caught ITK exception: " << e << std::endl;
    delete extractSlice;
    return 1;
  }
  
  /** Return a value. */
  return 0;

} // end main
