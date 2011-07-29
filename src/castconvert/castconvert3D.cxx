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
#include "castconverthelpers.h"


void ITKToolsCastConvert3D(
  itktools::ComponentType outputComponentType, unsigned int dim,
  ITKToolsCastConvertBase * & castConvert )
{
  if (!castConvert) castConvert = ITKToolsCastConvert< short, 3 >::New( outputComponentType, dim );
  if (!castConvert) castConvert = ITKToolsCastConvert< unsigned short, 3 >::New( outputComponentType, dim );
  if (!castConvert) castConvert = ITKToolsCastConvert< char, 3 >::New( outputComponentType, dim );
  if (!castConvert) castConvert = ITKToolsCastConvert< unsigned char, 3 >::New( outputComponentType, dim );
  if (!castConvert) castConvert = ITKToolsCastConvert< int, 3 >::New( outputComponentType, dim );
  if (!castConvert) castConvert = ITKToolsCastConvert< unsigned int, 3 >::New( outputComponentType, dim );
  if (!castConvert) castConvert = ITKToolsCastConvert< long, 3 >::New( outputComponentType, dim );
  if (!castConvert) castConvert = ITKToolsCastConvert< unsigned long, 3 >::New( outputComponentType, dim );
  if (!castConvert) castConvert = ITKToolsCastConvert< float, 3 >::New( outputComponentType, dim );
  if (!castConvert) castConvert = ITKToolsCastConvert< double, 3 >::New( outputComponentType, dim );

} // end ITKToolsCastConvert3D()
