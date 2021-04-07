<div class="simposiums-records">
  <?php if( isset($content) && !empty($content) ): ?> 
    <h2>Próximos Simposiums</h2>
    <div class="row">
      <div class="next-simposiums">
        <?php foreach($content as $simposio): ?>
          <div class="card border-info <?php print ($simposio['register'] == 1) ? 'card-true' : 'card-false'; ?>">  
            <img class="card-img-top" src="<?php print $simposio['img']; ?>" alt="Card image cap">
            <div class="card-body text-info">
              <h5 class="card-title"><?php print $simposio['title']; ?></h5>
              <p class="card-date"><?php print $simposio['date']; ?></p>
              <p class="card-text"><?php print $simposio['desc']; ?></p>
              <?php print $simposio['action']; ?>
            </div>
          </div>
        <?php endforeach; ?>
      </div>
    </div>
  <?php endif; ?>
</div>