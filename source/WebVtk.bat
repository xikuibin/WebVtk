REM This-is-a-wt-example
set PATH=%~p0; C:\Develop\VTK7.0.Opengl\bin;C:\Develop\Wt-3.3.6-msvs2015-Windows-x64-SDK\bin;%PATH%
set OLDPWD=%CD%
REM Required until all examples accept a "--approot path" parameter
cd ..\

"%~p0\WebVtk.exe" --docroot docroot  --http-port 8080 --http-addr 0.0.0.0 %*
