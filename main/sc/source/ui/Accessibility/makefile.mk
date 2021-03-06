#**************************************************************
#  
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#  
#    http://www.apache.org/licenses/LICENSE-2.0
#  
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#  
#**************************************************************



PRJ=..$/..$/..

PRJNAME=sc
TARGET=accessibility

# --- Settings -----------------------------------------------------

.INCLUDE :  scpre.mk
.INCLUDE :  settings.mk
.INCLUDE :  sc.mk
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# --- Files --------------------------------------------------------

SLOFILES =  										\
		$(SLO)$/AccessibleContextBase.obj			\
		$(SLO)$/AccessibleTableBase.obj				\
		$(SLO)$/AccessibleDocument.obj				\
		$(SLO)$/AccessibleGlobal.obj				\
		$(SLO)$/AccessibleSpreadsheet.obj			\
		$(SLO)$/AccessibleCell.obj					\
		$(SLO)$/AccessibilityHints.obj				\
		$(SLO)$/AccessibleDocumentBase.obj			\
		$(SLO)$/AccessibleCellBase.obj				\
		$(SLO)$/AccessibleDocumentPagePreview.obj	\
		$(SLO)$/AccessibleFilterMenu.obj	\
		$(SLO)$/AccessibleFilterMenuItem.obj	\
		$(SLO)$/AccessibleFilterTopWindow.obj	\
		$(SLO)$/AccessiblePreviewTable.obj			\
		$(SLO)$/AccessiblePreviewCell.obj			\
		$(SLO)$/AccessiblePreviewHeaderCell.obj		\
		$(SLO)$/AccessiblePageHeader.obj			\
		$(SLO)$/AccessibleText.obj					\
		$(SLO)$/AccessiblePageHeaderArea.obj		\
		$(SLO)$/DrawModelBroadcaster.obj			\
		$(SLO)$/AccessibleEditObject.obj			\
		$(SLO)$/AccessibleDataPilotControl.obj		\
		$(SLO)$/AccessibleCsvControl.obj

EXCEPTIONSFILES=									\
		$(SLO)$/AccessibleContextBase.obj			\
		$(SLO)$/AccessibleTableBase.obj				\
		$(SLO)$/AccessibleDocument.obj				\
		$(SLO)$/AccessibleGlobal.obj				\
		$(SLO)$/AccessibleSpreadsheet.obj			\
		$(SLO)$/AccessibleCell.obj					\
		$(SLO)$/AccessibleDocumentBase.obj			\
		$(SLO)$/AccessibleCellBase.obj				\
		$(SLO)$/AccessibleDocumentPagePreview.obj	\
		$(SLO)$/AccessibleFilterMenu.obj	\
		$(SLO)$/AccessibleFilterMenuItem.obj	\
		$(SLO)$/AccessibleFilterTopWindow.obj	\
		$(SLO)$/AccessiblePreviewTable.obj			\
		$(SLO)$/AccessiblePreviewCell.obj			\
		$(SLO)$/AccessiblePreviewHeaderCell.obj		\
		$(SLO)$/AccessiblePageHeader.obj			\
		$(SLO)$/AccessiblePageHeaderArea.obj		\
		$(SLO)$/DrawModelBroadcaster.obj			\
		$(SLO)$/AccessibleEditObject.obj			\
		$(SLO)$/AccessibleDataPilotControl.obj		\
		$(SLO)$/AccessibleCsvControl.obj

# --- Tagets -------------------------------------------------------

.INCLUDE :  target.mk


