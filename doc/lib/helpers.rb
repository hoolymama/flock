include Nanoc::Helpers::LinkTo

# Add more types if you need to, for example: Houdini SOP. 
# Then set the :type field in the yaml section at the top of each help file of that type
# Those pages will then appear in the sidebar menu
TYPES = ["Tutorial","Maya Node","Maya Plugin Command","Maya MEL Command","Maya MEL Script","Python Script", "Nuke Node", "Nuke Script"]

MODULE_TYPES = ["Index", "Release Notes", "Install Guide"]

# create markup for the header
def create_header
  body =  '<div id="header">'
  body += "<ul>"
  body += '<li class="brand">'
  body +=  @item[:product_name]
  body += '</li>'
  body += '<li class="tag">'
  body += "#{@item[:product_description]}"
  body += '</li>' 
  body += "</ul>" 
  body += "</div>" 
end

# create markup for the metadata
def create_meta

  title = @item[:title]

  body = '<div class="meta_container">' 

  body += '<div class="metainfo"><b>'
  body += "#{@item[:type]}: </b>" 
  body += "#{title}"
  body += '</div>' 

  @item[:description] = "Please write a description for this page" if @item[:description].nil?
  body += '<div class="metainfo"><b>Description: </b>' 
  body +=  @item[:description] 
  body += '</div>' 

  @item[:email] = "you@example.com"  if @item[:email].nil?
  @item[:author] = "Your Name"  if @item[:author].nil?
  body += '<div class="metainfo"><b>Author: </b><a href="mailto:' 
  body += @item[:email] 
  body += '">' 
  body += @item[:author]
  body +='</a></div>'


  body += '<div class="metainfo"><b>Created: </b>' +  @item[:created_at] + '</div>' if (! @item[:created_at].nil?)
  body += '<div class="metainfo"><b>Keywords: </b>' +  @item[:keywords] + '</div>' if (! @item[:keywords].nil?)
  body += '<div class="metainfo"><b>Plugin: </b>' +  @item[:plugin] + '</div>' if (! @item[:plugin].nil?)
  body += '<div class="metainfo"><b>Test Scene: </b>' +  @item[:test_scene] + '</div>' if (!  @item[:test_scene].nil?)

  body += '</div>' 

end

# This helper displays a webm (HTML5) movie in a div with a title and caption. 
# You can convert quicktime and other formats to webm with
# Miro Video Converter on Mac. http://blog.getmiro.com/2011/01/miro-video-converter-now-in-the-mac-app-store/
# For windows and linux you'll have to search. I believe there is also an online converter
def movie_tag(title,caption,movie,side)
  body=""
  body += '<div class="figure '+side+'">'
  body += "<video controls>" 
  body +=  '<source src="'  + movie +'" type="video/webm">'  
  body +=   "Sorry dude, your browser doesn't support HTML5 video. Upgrade already!"  
  body +=  "</video>"
  body +='<p class="title">'+title+'</p>'
  body +='<p class="caption">'+caption+'</p>'
  body +='</div>'
end





# This helper displays an image in a div with a title and caption. 
# The image is resized by css if it is too big, however it will be a hyperlink to the full-size image.
def figure_tag(title,caption,image,side)
  @figure_counter ||= 0 
  figure_name = "Figure " + @figure_counter.to_s+'.'
  @figure_counter+=1
  body=""
  body += '<div class="figure '+side+'">'
  body +=  '<a href="'+image+'"><img  src="'+image+'"  title="'+caption+'" /></a>'
  # body +='<img alt="Image missing" src="'+image+'" title="'+caption+'"/>'
  body +='<p class="title">'+figure_name+' '+title+'</p>'
  body +='<p class="caption">'+caption+'</p>'
  body +='</div>'
end

# create one block for the table of contents with the given array of pages and the heading
def toc_block(pages, heading)

  body =""
  if pages.length > 0
    body += '<div class="toc_block">'  
    body += "<h4>#{heading}</h4>" 
    body += '<ul>'

    pages.each do |page|
      display_text = page[:title]
      display_text = page[:type].capitalize if heading == "Overview"

      unless page == @item
        body += "<li>"
        if ( page.identifier == "/") 
          body += link_to(display_text , "index.html") 
        else
          body +=  link_to(display_text ,  relative_path_to(page)) 
        end
      else
        body += '<li class="current">'
        body += display_text
      end
      body += "</li>"
    end

    body += '</ul>'  

    
    body += '</div>'

  end
  
  body
end

# create markup for the table of contents in the sidebar.
def toc

  items =  @items.select { |i| (i[:title] != nil)  }
  body =""
  sub_pages = items.select { |i|  MODULE_TYPES.include?(i[:type]) }

  body += toc_block(sub_pages, "Overview")

  TYPES.each do |type| 
    sub_pages = items.select  { |i|  i[:type] == type }
    body +=  toc_block(sub_pages, "#{type}s")
  end
  body += '<hr>'  
  body
end


# def vfxoverflow_ribbon
#   r = "site_images/vfxoverflow_ribbon.png"
#   body =  '<a href="http://www.vfxoverflow.com/questions/new"><img id="vfxoverflow" src="'+r+'" alt="Ask at VFX Overflow" /></a>'
# end


# LOGOS = [
#   # [ "www.mirovideoconverter.com" , "miro_logo_bw.png", "miro video converter"],
#   [ "www.thefoundry.co.uk/support" , "nuke.png", "nuke support"],
#   [ "download.autodesk.com/global/docs/maya2014/en_us/index.html" , "maya.png", "maya 2012 help"],
#   [ "www.python.org/about/help" , "python.png", "python help"],
#   [ "www.chaosgroup.com/en/2/support.html" , "vray.png", "vray support"],
#   [ "renderman.pixar.com/forum/support.php" , "prman.png", "renderman support"]
# ]

def footer

 str = 	'<div id="footer">'
 y = Time.now.strftime("%Y")
 str += "<div>© #{y}</div>"

 #  str += "<div>#{@item[:product_name]} © #{y} #{@item[:author]}.</div>" 
 #  str += "<div>The content of the #{@item[:product_name]} documentation web site is licensed under a Creative Commons Attribution-ShareAlike 3.0 Unported License.</div>" 
 #  str += "<div>#{@item[:product_name]} is licenced under the BSD licence.</div>"
 #  str += '</div>'

 str

end


