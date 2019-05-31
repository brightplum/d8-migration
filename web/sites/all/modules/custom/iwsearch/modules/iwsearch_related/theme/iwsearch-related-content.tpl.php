<?php
if (isset($_GET['amp'])) {
  $rows = array_chunk($entities, 2);
}
else {
  $rows = array_chunk($entities, 4);
}
?>
<div<?php print $attributes; ?>>
  <div class="related-content-title clearfix"><h3>Related Articles</h3></div>

  <div<?php print $content_attributes; ?>>
    <?php foreach ($rows as $row) : ?>
        <div class="clearfix">
          <?php foreach ($row as $display): ?>
              <?php if (isset($display['entity']['type']) && $display['entity']['type'] == 'node') : ?>
                  <div<?php print drupal_attributes($display['attributes']);?>>
                      <article role="article" class="node node--<?php print $display['entity']['bundle']; ?> node--promoted node--sticky node--related node--<?php print $display['entity']['bundle']; ?>--related">
                        <div class="grid-size-12 node__content">
                            <?php if (isset($display['entity']['image'])) : ?>
                            <div class="field field--name-field-image field--type-image field--label-hidden">
                                <div class="field__items">
                                    <div class="field__item even">
                                        <?php print l($display['entity']['image'], 'node/' . $display['entity']['nid'], ['html' => TRUE]); ?>
                                    </div>
                                </div>
                            </div>
                            <?php endif; ?>
                            <?php if (isset($display['entity']['title'])) : ?>
                            <div class="field field--name-field-hp-title field--type-text field--label-hidden">
                                <div class="field__items">
                                    <div class="field__item even">
                                        <?php print l($display['entity']['title'], 'node/' . $display['entity']['nid']); ?>
                                    </div>
                                </div>
                            </div>
                            <?php endif; ?>
                        </div>
                      </article>
                  </div>
              <?php else : ?>
                  <div<?php print drupal_attributes($display['attributes']);?>>
                    <?php print render($display['entity']); ?>
                  </div>
              <?php endif; ?>
          <?php endforeach; ?>
        </div>
    <?php endforeach; ?>
  </div>
</div>
