@echo off
REM
REM Copyright (c) 2016-2017  Moddable Tech, Inc.
REM
REM   This file is part of the Moddable SDK Tools.
REM 
REM   The Moddable SDK Tools is free software: you can redistribute it and/or modify
REM   it under the terms of the GNU General Public License as published by
REM   the Free Software Foundation, either version 3 of the License, or
REM   (at your option) any later version.
REM 
REM   The Moddable SDK Tools is distributed in the hope that it will be useful,
REM   but WITHOUT ANY WARRANTY; without even the implied warranty of
REM   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
REM   GNU General Public License for more details.
REM 
REM   You should have received a copy of the GNU General Public License
REM   along with the Moddable SDK Tools.  If not, see <http://www.gnu.org/licenses/>.
REM
REM
SET MODDABLE=%~dp0\..\..\..
CALL "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" amd64
SET BUILD_DIR=%MODDABLE%\build
SET XS_DIR=%MODDABLE%\xs
SET VC_INCLUDE=%INCLUDE%
@echo on

nmake GOAL=debug BUILD_DIR=%BUILD_DIR% XS_DIR=%XS_DIR% VC_INCLUDE="%VC_INCLUDE%" /c /f xst.mak /s
nmake GOAL=release BUILD_DIR=%BUILD_DIR% XS_DIR=%XS_DIR% VC_INCLUDE="%VC_INCLUDE%" /c /f xst.mak /s
