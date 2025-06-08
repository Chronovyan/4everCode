@echo off
REM New Documentation Generator for Windows
REM This script creates a new documentation page with proper front matter and structure
REM SAFETY: This script will create new files and modify mkdocs.yml
REM Add --dry-run to see what would be created without making changes

setlocal enabledelayedexpansion

:: Initialize dry run flag
set DRY_RUN=false
if "%~1"=="--dry-run" (
    set DRY_RUN=true
    echo 🔍 DRY RUN MODE - No changes will be made
)

:: Function to safely create a file
:safe_create_file
    set "file_path=%~1"
    set "content=%~2"
    
    if "%DRY_RUN%"=="true" (
        echo [DRY RUN] Would create file: %file_path%
        echo --- START OF: %file_path% ---
        echo %content%
        echo --- END OF: %file_path% ---
        echo.
        goto :eof
    )
    
    :: Create directory if it doesn't exist
    for %%F in ("%file_path%") do set "dir_path=%%~dpF"
    if not exist "%dir_path%" mkdir "%dir_path%"
    
    :: Create file with content
    echo %content% > "%file_path%"
    echo ✅ Created file: %file_path%
    
    goto :eof

:: Function to safely update mkdocs.yml
:update_mkdocs_nav
    set "doc_path=%~1"
    set "doc_category=%~2"
    
    if "%DRY_RUN%"=="true" (
        echo [DRY RUN] Would update mkdocs.yml to include: %doc_category%/%doc_path%
        goto :eof
    )
    
    :: Backup mkdocs.yml if not already backed up
    if not exist "mkdocs.yml.bak" (
        copy "mkdocs.yml" "mkdocs.yml.bak" >nul
        echo ℹ️  Created backup: mkdocs.yml.bak
    )
    
    :: Update mkdocs.yml
    findstr /C:"%doc_category%/%doc_path%" mkdocs.yml >nul 2>&1
    if errorlevel 1 (
        powershell -Command "(Get-Content mkdocs.yml) -replace '  - %doc_category%:', '  - %doc_category%:`n      - %doc_category%/%doc_path%' | Set-Content mkdocs.yml -NoNewline"
        echo ✅ Updated mkdocs.yml navigation
    )
    
    goto :eof

:: Function to confirm before proceeding
:confirm
    set "message=%~1"
    set "default=%~2"
    
    if "%DRY_RUN%"=="true" (
        goto :eof
    )
    
    if "%default%"=="y" (
        set "prompt=%message% [Y/n] "
    ) else (
        set "prompt=%message% [y/N] "
    )
    
    set /p "response=%prompt%"
    if /i "%response%"=="" set "response=%default%"
    
    if /i "%response%"=="y" (
        goto :eof
    ) else (
        echo Operation cancelled by user
        exit /b 1
    )

:: Colors (not supported in standard cmd, but will work in terminals that support ANSI)
set GREEN=[32m
set YELLOW=[33m
set RED=[31m
set CYAN=[36m
set NC=[0m

:: Function to print section headers
:print_section
    setlocal
    set "title=%~1"
    set "width=80"
    
    echo.
    echo %CYAN%%title%%NC%
    echo %CYAN%================================================================================%NC%
    echo.
    endlocal
goto :eof

:: Function to prompt for input with a default value
:prompt
    setlocal
    set "prompt=%~1"
    set "default=%~2"
    set "var=%~3"
    
    if not "%default%"=="" (
        set /p "!var!=%prompt% [%default%]: "
        if "!%var%!"=="" set "!var!=%default%"
    ) else (
        :prompt_required
        set /p "!var!=%prompt%: "
        if "!%var%!"=="" (
            echo %YELLOW%This field is required.%NC%
            goto prompt_required
        )
    )
    endlocal
goto :eof

:: Function to generate a slug from a title
:generate_slug
    setlocal
    set "title=%~1"
    setlocal enabledelayedexpansion
    
    :: Convert to lowercase
    set "slug=!title: =_!"
    
    :: Replace spaces with hyphens and remove special characters
    set "slug=!slug: =_!"
    set "slug=!slug:^&=!"
    set "slug=!slug:^|=!"
    set "slug=!slug:^<=!"
    set "slug=!slug:^>=!"
    set "slug=!slug:^[=!"
    set "slug=!slug:^]=!"
    set "slug=!slug:^;=!"
    set "slug=!slug:^,=!"
    set "slug=!slug:^/=!"
    set "slug=!slug:^\=!"
    set "slug=!slug:^:=!"
    set "slug=!slug:^+=!"
    set "slug=!slug:^=?!"
    set "slug=!slug:^@=!"
    set "slug=!slug:^#=!"
    set "slug=!slug:^%%=!"
    set "slug=!slug:^^=!"
    set "slug=!slug:^~=!"
    set "slug=!slug:^*=!"
    set "slug=!slug:^(=!"
    set "slug=!slug:^)=!"
    set "slug=!slug:^_=!"
    set "slug=!slug:^'=!"
    set "slug=!slug:^"=!"
    
    :: Convert to lowercase
    set "slug=!slug:a=a!"
    set "slug=!slug:b=b!"
    set "slug=!slug:c=c!"
    set "slug=!slug:d=d!"
    set "slug=!slug:e=e!"
    set "slug=!slug:f=f!"
    set "slug=!slug:g=g!"
    set "slug=!slug:h=h!"
    set "slug=!slug:i=i!"
    set "slug=!slug:j=j!"
    set "slug=!slug:k=k!"
    set "slug=!slug:l=l!"
    set "slug=!slug:m=m!"
    set "slug=!slug:n=n!"
    set "slug=!slug:o=o!"
    set "slug=!slug:p=p!"
    set "slug=!slug:q=q!"
    set "slug=!slug:r=r!"
    set "slug=!slug:s=s!"
    set "slug=!slug:t=t!"
    set "slug=!slug:u=u!"
    set "slug=!slug:v=v!"
    set "slug=!slug:w=w!"
    set "slug=!slug:x=x!"
    set "slug=!slug:y=y!"
    set "slug=!slug:z=z!"
    
    :: Replace multiple underscores with a single one
    :underscore_loop
    if not "!slug:__=!"=="!slug!" (
        set "slug=!slug:__=_!"
        goto underscore_loop
    )
    
    :: Remove leading and trailing underscores
    if "!slug:~0,1!"=="_" set "slug=!slug:~1!"
    if "!slug:~-1!"=="_" set "slug=!slug:~0,-1!"
    
    endlocal & set "%~2=!slug!"
goto :eof

:: Main script
call :print_section "📄 New Chronovyan Documentation Page"

:: Get document details
set "DOC_TITLE="
set "DOC_DESCRIPTION="
set "DOC_CATEGORY="
set "DOC_WEIGHT=10"
set "DOC_AUTHOR=Chronovyan Team"

call :prompt "Document title" "" DOC_TITLE
call :prompt "Short description" "" DOC_DESCRIPTION
call :prompt "Category (guides/reference/development/design/community)" "" DOC_CATEGORY
call :prompt "Navigation weight" "10" DOC_WEIGHT
call :prompt "Author" "%DOC_AUTHOR%" DOC_AUTHOR

:: Generate slug from title
call :generate_slug "%DOC_TITLE%" DOC_SLUG
set "DOC_FILENAME=%DOC_SLUG%.md"

:: Determine the directory based on category
if /i "%DOC_CATEGORY%"=="guides" (
    set "DOC_DIR=docs\guides"
) else if /i "%DOC_CATEGORY%"=="reference" (
    set "DOC_DIR=docs\reference"
) else if /i "%DOC_CATEGORY%"=="development" (
    set "DOC_DIR=docs\development"
) else if /i "%DOC_CATEGORY%"=="design" (
    set "DOC_DIR=docs\design"
) else if /i "%DOC_CATEGORY%"=="community" (
    set "DOC_DIR=docs\community"
) else (
    echo %YELLOW%Invalid category. Using 'docs' as the default directory.%NC%
    set "DOC_DIR=docs"
)

:: Create the directory if it doesn't exist
if not exist "%DOC_DIR%" mkdir "%DOC_DIR%"

:: Generate the file path
set "DOC_PATH=%DOC_DIR%\%DOC_FILENAME%"

:: Check if file already exists
if exist "%DOC_PATH%" (
    if "%DRY_RUN%"=="true" (
        echo %YELLOW%[DRY RUN] Would check if file exists: %DOC_PATH%%NC%
    ) else (
        echo %YELLOW%Warning: File %DOC_PATH% already exists.%NC%
        set /p "OVERWRITE=Overwrite? [y/N] "
        if /i not "%OVERWRITE%"=="y" (
            echo %YELLOW%Aborted.%NC%
            exit /b 1
        )
    )
)

:: Get current date in ISO 8601 format
for /f "tokens=2 delims==" %%a in ('wmic OS Get localdatetime /value') do set "dt=%%a"
set "CURRENT_DATE=%dt:~0,4%-%dt:~4,2%-%dt:~6,2%"

:: Generate the document content
set "DOC_CONTENT=---"
set "DOC_CONTENT=!DOC_CONTENT!\ntitle: "%DOC_TITLE%""
set "DOC_CONTENT=!DOC_CONTENT!\ndescription: "%DOC_DESCRIPTION%""
set "DOC_CONTENT=!DOC_CONTENT!\ndate: %CURRENT_DATE%"
set "DOC_CONTENT=!DOC_CONTENT!\nweight: %DOC_WEIGHT%"
set "DOC_CONTENT=!DOC_CONTENT!\nauthors: ["%DOC_AUTHOR%"]"
set "DOC_CONTENT=!DOC_CONTENT!\n---\n"
set "DOC_CONTENT=!DOC_CONTENT!\n# %DOC_TITLE%\n"
set "DOC_CONTENT=!DOC_CONTENT!\n%DOC_DESCRIPTION%\n"
set "DOC_CONTENT=!DOC_CONTENT!\n## Overview\n"
set "DOC_CONTENT=!DOC_CONTENT!\nAdd a brief overview of what this document covers.\n"
set "DOC_CONTENT=!DOC_CONTENT!\n## Prerequisites\n"
set "DOC_CONTENT=!DOC_CONTENT!\n- Prerequisite 1"
set "DOC_CONTENT=!DOC_CONTENT!\n- Prerequisite 2\n"
set "DOC_CONTENT=!DOC_CONTENT!\n## Instructions\n"
set "DOC_CONTENT=!DOC_CONTENT!\n1. Step 1"
set "DOC_CONTENT=!DOC_CONTENT!\n2. Step 2"
set "DOC_CONTENT=!DOC_CONTENT!\n3. Step 3\n"
set "DOC_CONTENT=!DOC_CONTENT!\n## Examples\n"
set "DOC_CONTENT=!DOC_CONTENT!\n```rust"
set "DOC_CONTENT=!DOC_CONTENT!\n// Example code here"
set "DOC_CONTENT=!DOC_CONTENT!\nfn main() {"
set "DOC_CONTENT=!DOC_CONTENT!\n    println!(""Hello, Chronovyan!"");"
set "DOC_CONTENT=!DOC_CONTENT!\n}"
set "DOC_CONTENT=!DOC_CONTENT!\n```\n"
set "DOC_CONTENT=!DOC_CONTENT!\n## Troubleshooting\n"
set "DOC_CONTENT=!DOC_CONTENT!\n| Issue | Solution |"
set "DOC_CONTENT=!DOC_CONTENT!\n|-------|----------|"
set "DOC_CONTENT=!DOC_CONTENT!\n| Common issue 1 | Solution 1 |"
set "DOC_CONTENT=!DOC_CONTENT!\n| Common issue 2 | Solution 2 |\n"
set "DOC_CONTENT=!DOC_CONTENT!\n## See Also\n"
set "DOC_CONTENT=!DOC_CONTENT!\n- [Related Document 1]()"
set "DOC_CONTENT=!DOC_CONTENT!\n- [Related Document 2]()"

:: Create the document
call :safe_create_file "%DOC_PATH%" "!DOC_CONTENT!"

:: Update mkdocs.yml if the category exists
if exist "mkdocs.yml" (
    if not "%DOC_CATEGORY%"=="" (
        if exist "docs\%DOC_CATEGORY%" (
            findstr /C:"%DOC_CATEGORY%\\%DOC_FILENAME:" mkdocs.yml >nul 2>&1
            if errorlevel 1 (
                call :confirm "Add '%DOC_CATEGORY%/%DOC_FILENAME%' to mkdocs.yml navigation?" "y"
                call :update_mkdocs_nav "%DOC_FILENAME:.md=%" "%DOC_CATEGORY%"
            )
        )
    )
)

if "%DRY_RUN%"=="false" (
    echo.
    echo %GREEN%✓ Created new documentation page at: %DOC_PATH%%NC%
) else (
    echo.
    echo %YELLOW%✓ Dry run complete. No files were modified.%NC%
)
echo.
echo Next steps:
echo 1. Edit the file to add your content
echo 2. Preview with: %CYAN%mkdocs serve%NC%
echo 3. Commit your changes
echo.

exit /b 0
