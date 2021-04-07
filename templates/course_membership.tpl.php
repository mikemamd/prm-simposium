<?php if(!empty($content)): ?>
  <div id="block-views-courses-courses-vip" class="block block-views contextual-links-region clearfix">
    <h3 style="background: #33CDF7 !important;">Edicación Continua [Membresía]</h3>
    <div class="content">
      <div class="view view-courses view-id-courses view-display-id-courses_vip">
        <div class="view-header">
          <p style="padding: inherit;">Elige algunas de nuestras plataformas para conocer nuestros cursos adquiridos con membresía</p>
        </div>
        <div class="view-content">
          <div class="owl-carousel-courses_vip10 owl-carousel owl-theme" style="opacity: 1; display: block;">
            <div class="owl-wrapper-outer">
              <div class="owl-wrapper" style="width: 2680px; left: 0px; display: block;">
                <?php $i = 0; ?>
                <?php foreach($content as $course): ?>
                  <div class="owl-item active" style="width: 100%;">
                    <div class="item-<?php print $i; ?> item-odd">
                      <?php if( isset($course['image']) && !empty($course['image']) ): ?>
                        <img typeof="foaf:Image" src="<?php print $course['image']; ?>" alt="">
                      <?php endif; ?>
                      <div style="display: none; margin-top: 270px;"><h4>&nbsp;</h4>
                        <h4>&nbsp;</h4>
                        <h4><b><?php print $course['title']; ?></b></h4>
                        <?php if( isset($course['upath']) && !empty($course['upath']) ): ?> 
                          <a href="<?php print $course['upath']; ?>"><?php print $course['tpath']; ?></a>
                        <?php endif; ?>
                    </div>
                  </div>
                  </div>
                  <?php ++$i; ?>
                <?php endforeach; ?>  
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>    
<?php endif; ?>