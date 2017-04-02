import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.Toolkit;
import java.awt.geom.AffineTransform;
import java.awt.image.AffineTransformOp;
import java.awt.image.BufferedImage;
import java.awt.image.FilteredImageSource;
import java.awt.image.ImageFilter;
import java.awt.image.RGBImageFilter;
import java.io.File;
import java.io.IOException;
import java.util.Random;

import javax.imageio.ImageIO;
import javax.swing.JFrame;
import javax.swing.JOptionPane;

public class Main extends JFrame {
	
	public void paint(Graphics g) {
		g.setColor(Color.GREEN);

		g.fillOval(240, 240, 200, 100);
	}

	public static BufferedImage bufferRandTerrain(String filePath, String fileIndic, String fileType, int sampleCount) {
		BufferedImage img = null;

		Random rand = new Random();
		int n = rand.nextInt(sampleCount);

		try {
			img = ImageIO.read(new File(filePath + fileIndic + n + fileType));
		} catch (IOException e) {
			System.out.println("didn't buffer" + n);
		}
		return img;
	}

	public static BufferedImage printShape(BufferedImage buffer, String shapeFilePath, String shapeFileIndic,
			String fileType, int shapeId, int charCount, String characterFilePath, String charFileIndic) {

		final int MAXSCALE = 1;
		// buffers shape
		Random rand = new Random();
		Graphics2D g2d = buffer.createGraphics();
		BufferedImage shape = null;
		try {
			shape = ImageIO.read(new File(shapeFilePath + shapeFileIndic + shapeId + fileType));
		} catch (IOException e) {
			System.out.println("catch this");
		}
		//adjusts color of shape
		int redIncrement = -rand.nextInt(255);
		int greenIncrement = -rand.nextInt(255);
		int blueIncrement = -rand.nextInt(255);

		ImageFilter shapeFilter = new RGBImageFilter() {
			@Override
			public int filterRGB(int x, int y, int rgb) {
				int alpha = (rgb & 0xff000000);
				int red = (rgb & 0xff0000) >> 16;
				int green = (rgb & 0x00ff00) >> 8;
				int blue = (rgb & 0x0000ff);

				red = Math.max(0, Math.min(0xff, red + redIncrement));
				green = Math.max(0, Math.min(0xff, green + greenIncrement));
				blue = Math.max(0, Math.min(0xff, blue + blueIncrement));

				return alpha | (red << 16) | (green << 8) | blue;
			}
		};

		Image filteredImage = Toolkit.getDefaultToolkit()
				.createImage(new FilteredImageSource(shape.getSource(), shapeFilter));
		
		//rotates shape
		double rotateAngle = (rand.nextDouble() + rand.nextInt(7)) % 6.28;
		
		//prepares scale with random elements
		BufferedImage shapeRot = rotate(toBufferedImage(filteredImage), rotateAngle);
		double randScale = 0;
		while (randScale < 0.3) {
			randScale = rand.nextInt(MAXSCALE) + rand.nextDouble();
		}
		double minBuild = 0;
		while (minBuild < 0.4) {
			minBuild = rand.nextInt(MAXSCALE) + rand.nextDouble();
		}
		int xpos = -1000;
		int ypos = -1000;

		while (xpos > (buffer.getWidth() - shapeRot.getWidth()) || xpos < 0) {
			xpos = rand.nextInt(buffer.getWidth()) - rand.nextInt(shapeRot.getWidth());
		}

		while (ypos > (buffer.getHeight() - shapeRot.getHeight()) || ypos < 0) {
			ypos = rand.nextInt(buffer.getHeight()) - rand.nextInt(shapeRot.getHeight());
		}

		double widthExtraScale = 0;
		double heightExtraScale = 0;

		while (widthExtraScale > 0 && widthExtraScale < 0.3) {
			widthExtraScale = rand.nextDouble();
		}

		while (heightExtraScale > 0 && heightExtraScale < 0.3) {
			heightExtraScale = rand.nextDouble();
		}

		
		g2d.setFont(new Font("TimesRoman", Font.PLAIN, 50));
		int widthScale = (int) (shapeRot.getWidth() * (randScale) * (widthExtraScale + minBuild));
		int heightScale = (int) (shapeRot.getHeight() * (randScale) * (heightExtraScale + minBuild));
		
		g2d.drawImage(shapeRot, xpos, ypos, widthScale,	heightScale, null);

		// creates random character
		int charNum = rand.nextInt(charCount);
		
		BufferedImage character = null;
		try {
			character = ImageIO.read(new File(characterFilePath + charFileIndic + charNum + fileType));
		} catch (IOException e) {
			System.out.println("catch this");
		}
		
		//adjusts color of random character
		int charRedIncrement = -rand.nextInt(255);
		int charGreenIncrement = -rand.nextInt(255);
		int charBlueIncrement = -rand.nextInt(255);

		ImageFilter charFilter = new RGBImageFilter() {
			@Override
			public int filterRGB(int x, int y, int rgb) {
				int alpha = (rgb & 0xff000000);
				int red = (rgb & 0xff0000) >> 16;
				int green = (rgb & 0x00ff00) >> 8;
				int blue = (rgb & 0x0000ff);

				red = Math.max(0, Math.min(0xff, red + charRedIncrement));
				green = Math.max(0, Math.min(0xff, green + charGreenIncrement));
				blue = Math.max(0, Math.min(0xff, blue + charBlueIncrement));

				return alpha | (red << 16) | (green << 8) | blue;
			}
		};
		
		Image charFilteredImage = Toolkit.getDefaultToolkit()
				.createImage(new FilteredImageSource(character.getSource(), charFilter));
		
		//prints character
		g2d.drawImage(rotate(toBufferedImage(charFilteredImage), rotateAngle), xpos + (int)(widthScale/3.3), ypos+(int)(heightScale/3.3), widthScale/3, heightScale/3, null);
		
		return buffer;
		
	}
	
	public static BufferedImage printChar(BufferedImage buffer, String shapeFilePath, String shapeFileIndic,
			String fileType, int charId, int shapeCount, String characterFilePath, String charFileIndic) {
		// buffer.getHeight();
		// buffer.getWidth();

		final int MAXSCALE = 1;
		// buffers shape
		Random rand = new Random();
		Graphics2D g2d = buffer.createGraphics();
		BufferedImage shape = null;
		int shapeNum = rand.nextInt(shapeCount-1);
		try {
			shape = ImageIO.read(new File(shapeFilePath + shapeFileIndic + shapeNum + fileType));
		} catch (IOException e) {
			System.out.println("catch this");
		}

		//adjusts color of shape
		int redIncrement = -rand.nextInt(255);
		int greenIncrement = -rand.nextInt(255);
		int blueIncrement = -rand.nextInt(255);

		ImageFilter shapeFilter = new RGBImageFilter() {
			@Override
			public int filterRGB(int x, int y, int rgb) {
				int alpha = (rgb & 0xff000000);
				int red = (rgb & 0xff0000) >> 16;
				int green = (rgb & 0x00ff00) >> 8;
				int blue = (rgb & 0x0000ff);

				red = Math.max(0, Math.min(0xff, red + redIncrement));
				green = Math.max(0, Math.min(0xff, green + greenIncrement));
				blue = Math.max(0, Math.min(0xff, blue + blueIncrement));

				return alpha | (red << 16) | (green << 8) | blue;
			}
		};

		Image filteredImage = Toolkit.getDefaultToolkit()
				.createImage(new FilteredImageSource(shape.getSource(), shapeFilter));
		
		//rotates shape
		double rotateAngle = (rand.nextDouble() + rand.nextInt(7)) % 6.28;
		
		BufferedImage shapeRot = rotate(toBufferedImage(filteredImage), rotateAngle);
		
		//generates random scaling
		double randScale = 0;
		while (randScale < 0.3) {
			randScale = rand.nextInt(MAXSCALE) + rand.nextDouble();
		}
		double minBuild = 0;
		while (minBuild < 0.4) {
			minBuild = rand.nextInt(MAXSCALE) + rand.nextDouble();
		}
		int xpos = -1000;
		int ypos = -1000;

		while (xpos > (buffer.getWidth() - shapeRot.getWidth()) || xpos < 0) {
			xpos = rand.nextInt(buffer.getWidth()) - rand.nextInt(shapeRot.getWidth());
		}

		while (ypos > (buffer.getHeight() - shapeRot.getHeight()) || ypos < 0) {
			ypos = rand.nextInt(buffer.getHeight()) - rand.nextInt(shapeRot.getHeight());
		}

		double widthExtraScale = 0;
		double heightExtraScale = 0;

		while (widthExtraScale > 0 && widthExtraScale < 0.3) {
			widthExtraScale = rand.nextDouble();
		}

		while (heightExtraScale > 0 && heightExtraScale < 0.3) {
			heightExtraScale = rand.nextDouble();
		}

		int widthScale = (int) (shapeRot.getWidth() * (randScale) * (widthExtraScale + minBuild));
		int heightScale = (int) (shapeRot.getHeight() * (randScale) * (heightExtraScale + minBuild));
		
		g2d.drawImage(shapeRot, xpos, ypos, widthScale,	heightScale, null);


		BufferedImage character = null;
		try {
			character = ImageIO.read(new File(characterFilePath + charFileIndic + charId + fileType));
		} catch (IOException e) {
			System.out.println("catch this");
		}
		
		//changes color for character
		int charRedIncrement = -rand.nextInt(255);
		int charGreenIncrement = -rand.nextInt(255);
		int charBlueIncrement = -rand.nextInt(255);

		ImageFilter charFilter = new RGBImageFilter() {
			@Override
			public int filterRGB(int x, int y, int rgb) {
				int alpha = (rgb & 0xff000000);
				int red = (rgb & 0xff0000) >> 16;
				int green = (rgb & 0x00ff00) >> 8;
				int blue = (rgb & 0x0000ff);

				red = Math.max(0, Math.min(0xff, red + charRedIncrement));
				green = Math.max(0, Math.min(0xff, green + charGreenIncrement));
				blue = Math.max(0, Math.min(0xff, blue + charBlueIncrement));

				return alpha | (red << 16) | (green << 8) | blue;
			}
		};
		
		Image charFilteredImage = Toolkit.getDefaultToolkit()
				.createImage(new FilteredImageSource(character.getSource(), charFilter));
		
		g2d.drawImage(rotate(toBufferedImage(charFilteredImage), rotateAngle), xpos + (int)(widthScale/3.3), ypos+(int)(heightScale/3.3), widthScale/3, heightScale/3, null);
		
		return buffer;
		
	}

	public static void main(String[] args) {
		
		// file indicator for all images
		String inFileIndic = "terrain";
		String filePathTerrain = "Enter Path ---\\JavaFiles\\ImageCreator\\terrain\\";
		String filePathShape = "Enter Path ---\\JavaFiles\\ImageCreator\\shape\\";
		String filePathChar = "Enter Path ---\\JavaFiles\\ImageCreator\\characters\\";

		String fileType = ".png";
		String shapeFileIndic = "shape";
		String outFileIndic = "practice";
		String charFileIndic = "let";
		
		boolean shape = false;
		int shapeId= 0;
		int charId = 0;
		int replyShapes = JOptionPane.showConfirmDialog(null, "Are you Printing Shapes? (Yes for shapes, No for characters)", "confirm", JOptionPane.YES_NO_OPTION);
	    if (replyShapes == JOptionPane.YES_OPTION) {
	       	shape = true;
			shapeId = Integer.parseInt(JOptionPane.showInputDialog("What is the shape ID of the shape you want to print?"));
	    }else{
	    	shape = false;
	    	charId = Integer.parseInt(JOptionPane.showInputDialog("What is the character ID of the character you want to print?"));
	    }
		
		// number of samples (including zero)
		final int GENERATENUMBER = Integer.parseInt(JOptionPane.showInputDialog("How many pictures?"));
		// number of Background pic's you have
		final int TERRAINCOUNT = 1;
		
		
		// how many shape pictures do you have?
		final int SHAPECOUNT = 6;
		
		// how many shape pictures do you have?
		final int CHARCOUNT = 26;
		int reply;
		if(shape){
			 reply = JOptionPane.showConfirmDialog(null, "Please confirm the details: \n you want " + GENERATENUMBER + " pictures of shape" + shapeId + " with terrain from : \n "+ filePathTerrain +" \n with shapes from : \n "+ filePathShape , "confirm", JOptionPane.YES_NO_OPTION);
		}else{
			reply = JOptionPane.showConfirmDialog(null, "Please confirm the details: \n you want " + GENERATENUMBER + " pictures of character" + charId + " with terrain from : \n "+ filePathTerrain +" \n with characters from : \n "+ filePathChar, "confirm", JOptionPane.YES_NO_OPTION);
		}
		
	    if (reply == JOptionPane.NO_OPTION) {
	       	return;
	    }
	        
		
		if (shape) {
			System.out.println("Printing " + GENERATENUMBER + " shape pictures with shapeID " + shapeId);
			for (int i = 0; i < GENERATENUMBER; i++) {
				writeToFile(printShape(bufferRandTerrain(filePathTerrain, inFileIndic, fileType, TERRAINCOUNT),
						filePathShape, shapeFileIndic, fileType, shapeId, CHARCOUNT, filePathChar, charFileIndic), outFileIndic, i, fileType);
			}
		}else{
			System.out.println("Printing " + GENERATENUMBER + " character pictures with characterID " + charId);
			for (int i = 0; i < GENERATENUMBER; i++) {
				writeToFile(printChar(bufferRandTerrain(filePathTerrain, inFileIndic, fileType, TERRAINCOUNT),
						filePathShape, shapeFileIndic, fileType, charId, SHAPECOUNT, filePathChar, charFileIndic), outFileIndic, i, fileType);
			}
		}
	}

	public static void writeToFile(BufferedImage o, String outFileIndic, int fileNum, String fileType) {
		try {
			System.out.println(outFileIndic + fileNum + fileType);
			File outputfile = new File(outFileIndic + fileNum + fileType);
			ImageIO.write(o, "png", outputfile);
		} catch (IOException e) {
		}
	}

	public static BufferedImage rotate(BufferedImage img, double angle) {
		AffineTransform tx = new AffineTransform();
		tx.rotate(angle, img.getWidth() / 2, img.getHeight() / 2);

		AffineTransformOp op = new AffineTransformOp(tx, AffineTransformOp.TYPE_BILINEAR);
		img = op.filter(img, null);
		return img;
	}

	public static BufferedImage toBufferedImage(Image img) {
		if (img instanceof BufferedImage) {
			return (BufferedImage) img;
		}

		// Create a buffered image with transparency
		BufferedImage bimage = new BufferedImage(img.getWidth(null), img.getHeight(null), BufferedImage.TYPE_INT_ARGB);

		// Draw the image on to the buffered image
		Graphics2D bGr = bimage.createGraphics();
		bGr.drawImage(img, 0, 0, null);
		bGr.dispose();

		// Return the buffered image
		return bimage;
	}
}
