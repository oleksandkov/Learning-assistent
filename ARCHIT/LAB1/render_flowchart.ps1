using namespace System.Drawing
using namespace System.Drawing.Drawing2D
using namespace System.Drawing.Imaging

Add-Type -AssemblyName System.Drawing

$width = 1200
$height = 1750
$bmp = [Bitmap]::new($width, $height)
$g = [Graphics]::FromImage($bmp)

$g.SmoothingMode = [SmoothingMode]::AntiAlias
$g.TextRenderingHint = [System.Drawing.Text.TextRenderingHint]::ClearTypeGridFit
$g.InterpolationMode = [InterpolationMode]::HighQualityBicubic

# Background
$bgBrush = [SolidBrush]::new([Color]::FromArgb(248, 250, 252))
$g.FillRectangle($bgBrush, 0, 0, $width, $height)

# Header Title
$titleFont = [Font]::new("Arial", 18, [FontStyle]::Bold)
$subTitleFont = [Font]::new("Arial", 11, [FontStyle]::Regular)
$headerBrush = [SolidBrush]::new([Color]::FromArgb(15, 23, 42))
$subHeaderBrush = [SolidBrush]::new([Color]::FromArgb(100, 116, 139))

$titleFormat = [StringFormat]::new()
$titleFormat.Alignment = [StringAlignment]::Center
$titleFormat.LineAlignment = [StringAlignment]::Center

$g.DrawString("БЛОК-СХЕМА АЛГОРИТМУ ПЕРЕВЕДЕННЯ", $titleFont, $headerBrush, ($width / 2), 45, $titleFormat)
$g.DrawString("двійкового числа (цілого або дробового) у десяткову систему числення", $subTitleFont, $subHeaderBrush, ($width / 2), 80, $titleFormat)

# Helpers for drawing shapes
$penBorder = [Pen]::new([Color]::FromArgb(51, 65, 85), 2)
$penArrow = [Pen]::new([Color]::FromArgb(71, 85, 105), 2.5)
$penArrow.CustomEndCap = [AdjustableArrowCap]::new(5, 6, $true)

$nodeFont = [Font]::new("Arial", 11, [FontStyle]::Regular)
$nodeBoldFont = [Font]::new("Arial", 11, [FontStyle]::Bold)
$labelFont = [Font]::new("Arial", 10, [FontStyle]::Bold)
$textBrush = [SolidBrush]::new([Color]::FromArgb(30, 41, 59))
$yesBrush = [SolidBrush]::new([Color]::FromArgb(22, 163, 74))
$noBrush = [SolidBrush]::new([Color]::FromArgb(220, 38, 38))

function Draw-RoundedRect($gr, [Rectangle]$r, [int]$d, $brush, $pen) {
    $path = [GraphicsPath]::new()
    $path.AddArc($r.X, $r.Y, $d, $d, 180, 90)
    $path.AddArc($r.X + $r.Width - $d, $r.Y, $d, $d, 270, 90)
    $path.AddArc($r.X + $r.Width - $d, $r.Y + $r.Height - $d, $d, $d, 0, 90)
    $path.AddArc($r.X, $r.Y + $r.Height - $d, $d, $d, 90, 90)
    $path.CloseFigure()
    $gr.FillPath($brush, $path)
    $gr.DrawPath($pen, $path)
}

function Draw-Parallelogram($gr, [int]$x, [int]$y, [int]$w, [int]$h, [int]$skew, $brush, $pen) {
    $path = [GraphicsPath]::new()
    [Point[]]$pts = @(
        [Point]::new($x + $skew, $y),
        [Point]::new($x + $w, $y),
        [Point]::new($x + $w - $skew, $y + $h),
        [Point]::new($x, $y + $h)
    )
    $path.AddPolygon($pts)
    $gr.FillPath($brush, $path)
    $gr.DrawPath($pen, $path)
}

function Draw-Diamond($gr, [int]$cx, [int]$cy, [int]$w, [int]$h, $brush, $pen) {
    $path = [GraphicsPath]::new()
    $hw = [int]($w / 2)
    $hh = [int]($h / 2)
    [Point[]]$pts = @(
        [Point]::new($cx, $cy - $hh),
        [Point]::new($cx + $hw, $cy),
        [Point]::new($cx, $cy + $hh),
        [Point]::new($cx - $hw, $cy)
    )
    $path.AddPolygon($pts)
    $gr.FillPath($brush, $path)
    $gr.DrawPath($pen, $path)
}

# Brushes for block types
$startBrush = [SolidBrush]::new([Color]::FromArgb(219, 234, 254))   # Blue-100
$inputBrush = [SolidBrush]::new([Color]::FromArgb(254, 243, 199))   # Amber-100
$processBrush = [SolidBrush]::new([Color]::FromArgb(241, 245, 249)) # Slate-100
$decisionBrush = [SolidBrush]::new([Color]::FromArgb(254, 226, 226))# Red-100
$outputBrush = [SolidBrush]::new([Color]::FromArgb(220, 252, 231))  # Green-100
$errorBrush = [SolidBrush]::new([Color]::FromArgb(254, 202, 202))

$centerX = 600

# 1. Start Block (Y: 130, H: 50, W: 240)
$r1 = [Rectangle]::new(($centerX - 120), 130, 240, 50)
Draw-RoundedRect $g $r1 40 $startBrush $penBorder
$g.DrawString("ПОЧАТОК", $nodeBoldFont, $textBrush, $centerX, 155, $titleFormat)

# Arrow 1 -> 2
$g.DrawLine($penArrow, $centerX, 180, $centerX, 215)

# 2. Input Block (Y: 215, H: 55, W: 420)
Draw-Parallelogram $g ($centerX - 210) 215 420 55 25 $inputBrush $penBorder
$g.DrawString("Введення двійкового числа S`n(цілого або дробового)", $nodeFont, $textBrush, $centerX, 242, $titleFormat)

# Arrow 2 -> 3
$g.DrawLine($penArrow, $centerX, 270, $centerX, 305)

# 3. Normalization & Sign Process (Y: 305, H: 65, W: 460)
$r3 = [Rectangle]::new(($centerX - 230), 305, 460, 65)
Draw-RoundedRect $g $r3 10 $processBrush $penBorder
$g.DrawString("Видалення пробілів, заміна ',' на '.'`nВизначення знаку (+/-) та очищення від нього`nРозбиття рядка на S_int та S_frac", $nodeFont, $textBrush, $centerX, 337, $titleFormat)

# Arrow 3 -> 4
$g.DrawLine($penArrow, $centerX, 370, $centerX, 410)

# 4. Decision: Validation (CenterY: 480, W: 520, H: 90)
Draw-Diamond $g $centerX 480 520 90 $decisionBrush $penBorder
$g.DrawString("Валідація:`nРядок містить тільки '0' і '1',`nта не більше однієї крапки?", $nodeFont, $textBrush, $centerX, 480, $titleFormat)

# Branch NO (Right)
$g.DrawLine($penArrow, ($centerX + 260), 480, ($centerX + 360), 480)
$g.DrawString("Ні (Помилка)", $labelFont, $noBrush, ($centerX + 265), 460)

# Error output block (X: centerX+360, Y: 450, W: 200, H: 60)
Draw-Parallelogram $g ($centerX + 360) 450 200 60 15 $errorBrush $penBorder
$g.DrawString("Виведення помилки:`n«Некоректне число»", $nodeFont, $textBrush, ($centerX + 460), 480, $titleFormat)

# Error to End line
$penSimple = [Pen]::new([Color]::FromArgb(71, 85, 105), 2.5)
$g.DrawLine($penSimple, ($centerX + 460), 510, ($centerX + 460), 1635)
$g.DrawLine($penArrow, ($centerX + 460), 1635, ($centerX + 120), 1635)

# Branch YES (Down)
$g.DrawLine($penArrow, $centerX, 525, $centerX, 580)
$g.DrawString("Так", $labelFont, $yesBrush, ($centerX + 10), 540)

# 5. Initialization (Y: 580, H: 70, W: 460)
$r5 = [Rectangle]::new(($centerX - 230), 580, 460, 70)
Draw-RoundedRect $g $r5 10 $processBrush $penBorder
$g.DrawString("Ініціалізація:`nD_int = 0;   D_frac = 0`nN = довжина(S_int);   M = довжина(S_frac)", $nodeFont, $textBrush, $centerX, 615, $titleFormat)

# Arrow 5 -> 6
$g.DrawLine($penArrow, $centerX, 650, $centerX, 690)

# 6. Loop for Integer Part (Y: 690, H: 90, W: 500)
$r6 = [Rectangle]::new(($centerX - 250), 690, 500, 90)
Draw-RoundedRect $g $r6 10 $processBrush $penBorder
$g.DrawString("Обчислення цілої частини (i від 0 до N-1):`npower = N - 1 - i;  bit = S_int[i]`nD_int = D_int + bit · 2^power`nЗапис доданку bit·2^power у формулу розкладу", $nodeFont, $textBrush, $centerX, 735, $titleFormat)

# Arrow 6 -> 7
$g.DrawLine($penArrow, $centerX, 780, $centerX, 825)

# 7. Decision: Has Fractional Part? (CenterY: 890, W: 460, H: 85)
Draw-Diamond $g $centerX 890 460 85 $decisionBrush $penBorder
$g.DrawString("Чи задано дробову частину?`n(довжина S_frac > 0)", $nodeFont, $textBrush, $centerX, 890, $titleFormat)

# Branch YES for Fraction (Down)
$g.DrawLine($penArrow, $centerX, 932, $centerX, 995)
$g.DrawString("Так", $labelFont, $yesBrush, ($centerX + 10), 950)

# 8. Loop for Fractional Part (Y: 995, H: 90, W: 500)
$r8 = [Rectangle]::new(($centerX - 250), 995, 500, 90)
Draw-RoundedRect $g $r8 10 $processBrush $penBorder
$g.DrawString("Обчислення дробової частини (j від 1 до M):`npower = -j;  bit = S_frac[j-1]`nD_frac = D_frac + bit · 2^(-j)`nЗапис доданку bit·2^(-j) у формулу розкладу", $nodeFont, $textBrush, $centerX, 1040, $titleFormat)

# Branch NO for Fraction (Left detour)
$g.DrawLine($penSimple, ($centerX - 230), 890, ($centerX - 330), 890)
$g.DrawString("Ні", $labelFont, $noBrush, ($centerX - 270), 870)
$g.DrawLine($penSimple, ($centerX - 330), 890, ($centerX - 330), 1130)
$g.DrawLine($penArrow, ($centerX - 330), 1130, ($centerX - 230), 1130)

# Arrow 8 -> 9
$g.DrawLine($penArrow, $centerX, 1085, $centerX, 1130)

# 9. Total calculation (Y: 1130, H: 75, W: 460)
$r9 = [Rectangle]::new(($centerX - 230), 1130, 460, 75)
Draw-RoundedRect $g $r9 10 $processBrush $penBorder
$g.DrawString("Підсумовування та врахування знаку:`nD = D_int + D_frac`nЯкщо sign == '-' то D = -D`nФормування підсумкового рядка результату", $nodeFont, $textBrush, $centerX, 1167, $titleFormat)

# Arrow 9 -> 10
$g.DrawLine($penArrow, $centerX, 1205, $centerX, 1245)

# 10. Prepare Table and Breakdown Details (Y: 1245, H: 70, W: 480)
$r10 = [Rectangle]::new(($centerX - 240), 1245, 480, 70)
Draw-RoundedRect $g $r10 10 $processBrush $penBorder
$g.DrawString("Формування покрокової таблиці:`nРозряд (степінь), біт b_i, вага 2^i, внесок розряду`nта повної аналітичної формули полінома", $nodeFont, $textBrush, $centerX, 1280, $titleFormat)

# Arrow 10 -> 11
$g.DrawLine($penArrow, $centerX, 1315, $centerX, 1355)

# 11. Output Block (Y: 1355, H: 85, W: 520)
Draw-Parallelogram $g ($centerX - 260) 1355 520 85 25 $outputBrush $penBorder
$g.DrawString("Виведення на екран:`n1) Десяткового результату D_10`n2) Полінома розкладу N_10 = ∑ (b_i · 2^i)`n3) Детальної покрокової таблиці внесків", $nodeBoldFont, $textBrush, $centerX, 1397, $titleFormat)

# Arrow 11 -> 12 (End)
$g.DrawLine($penArrow, $centerX, 1440, $centerX, 1610)

# 12. End Block (Y: 1610, H: 50, W: 240)
$r12 = [Rectangle]::new(($centerX - 120), 1610, 240, 50)
Draw-RoundedRect $g $r12 40 $startBrush $penBorder
$g.DrawString("КІНЕЦЬ", $nodeBoldFont, $textBrush, $centerX, 1635, $titleFormat)

# Save image
$outputPath = "c:\Github\Learning-assistent\ARCHIT\LAB1\algorithm_flowchart.png"
$bmp.Save($outputPath, [ImageFormat]::Png)

$g.Dispose()
$bmp.Dispose()
Write-Output "Image successfully saved to $outputPath"
